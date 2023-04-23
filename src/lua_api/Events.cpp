#include "Events.h"

#include "_common.h"
#include "seq.h"
#include "sentinel.h"

#include "../EventListener.h"
#include "../EventDispatcher.h"
#include "../Logger.h"

#define FOO_LUA_TEST_YIELDS 0

namespace {

	using namespace foo;
	using namespace foo::lua;

	inline constexpr std::string_view SubsystemName{ "Events" };

	inline constexpr std::string_view REG_LISTENERS{ "events.listener" };

	inline constexpr extraspace<EventsSubsystem> get{ };


	/// @brief Invokes the lua callback function.
	int marshall_args(lua_State* L, const Event& e);


	/// @brief Internal event listener.
	class LuaInternalEventListener : public EventListener {
	private:

		static int log_and_return(lua_State* L) {
			log(SubsystemName, Logger::Error, lua_tostring(L, -1)); 
			lua_pushboolean(L, false); 
			lua_pushvalue(L, -2); 
			return 2; 
		}

	public:
		
		LuaInternalEventListener(lua_State* L, lua_Integer id, unsigned flags)
			: L{ L }
			, id_{ id }
			, flags_{ flags } {

		}

		void operator()(const Event& e) override {
			//stack_sentinel sentinel(L);
			
			// msgh, func, args...
			lua_pushcclosure(L, log_and_return, 0);
			push_my_function();
			const int nargs = marshall_args(L, e);

			// save necessary context
			save.e = e;

#if FOO_LUA_TEST_YIELDS
			// TODO workaround because idk why it calls abort with pcallk, but not pcall
			panic_workaround panic(L, throw_no_abort);
#endif
#if defined(FOO_LUA_EVENTS_YIELD_SUPPORTED) || FOO_LUA_TEST_YIELDS
			// If this throws, dispatcher catches it.
			const auto ctx = reinterpret_cast<ptrdiff_t>(this);
			//lua_pcallk(L, nargs, 0, sentinel + 1, ctx, continuation);
			lua_pcallk(L, nargs, 0, 0, ctx, continuation);

#else
			// do call
			if (lua_pcall(L, nargs, 0, 0) != LUA_OK) {
				logf(SubsystemName, Logger::Error, "Listener [{}]: {}", e.name, lua_tostring(L, -1));
				lua_pop(L, 1);
			}

			// pop msgh
			lua_pop(L, 1);
#endif
		}

		unsigned event_type() const noexcept override {
			return flags_;
		}

	private:

		void push_my_function() {
			get_registry_value(L, REG_LISTENERS);
			seq_get(L, -1, id_); // push "us" from registry
			lua_getiuservalue(L, -1, 1); // get our function
			lua_rotate(L, -3, 1);
			lua_pop(L, 2);
		}

		lua_State* L;		// Events subsystem execution thread
		lua_Integer id_;	// Our id in the reg[REG_LISTENERS] seq
		unsigned flags_;	// event mask

	private: // below here are the error handling mess

		struct savestate {
			Event e;
		} save;

#if FOO_LUA_TEST_YIELDS
		struct panic_workaround {
			lua_CFunction old_panic;
			lua_State* L;

			panic_workaround(lua_State* L, lua_CFunction new_panic)
				: old_panic{ lua_atpanic(L, new_panic) }
				, L{ L } {

			}

			~panic_workaround() {
				lua_atpanic(L, old_panic);
			}
		};

		[[noreturn]] static int throw_no_abort(lua_State* L) noexcept(false) {
			const char* errmsg = lua_tostring(L, -1);
			// we're escaping, so manually clean up state
			lua_settop(L, 0);
			// old_panic is also restored after this
			//throw EventException(errmsg);
			return 0;
		}
#endif
#if defined(FOO_LUA_EVENTS_YIELD_SUPPORTED) || FOO_LUA_TEST_YIELDS
		static int continuation(lua_State* L, int status, lua_KContext ctx) {
			auto that = reinterpret_cast<LuaInternalEventListener*>(ctx);
			logf(SubsystemName, Logger::Verbose, "{} Got an error broddah", that->id_);
			if (status == LUA_YIELD) {
				logf(SubsystemName, Logger::Verbose, "Resumed {}", that->id_);
			}
			return 2;
		}
#endif

	};

	int marshall_args(lua_State* L, const Event& e) {
		const int otop = lua_gettop(L);

		// f(type, timestamp, name, ...)
		lua_pushinteger(L, static_cast<lua_Integer>(e.type));
		lua_pushinteger(L, e.timestamp.microseconds());
		lua_pushlstring(L, e.name.data(), e.name.length());

		int nargs = 3;
		switch (e.type) {
			using enum Event::Type;
		case Key:
			lua_pushinteger(L, e.key.key);
			lua_pushinteger(L, e.key.code);
			lua_pushinteger(L, e.key.action);
			lua_pushinteger(L, e.key.mods);
			break;
		case CharInput:
			lua_pushinteger(L, e.codept);
			break;
		case CursorEnter:
			lua_pushboolean(L, e.entered);
			break;
		case CursorPos:
			lua_pushnumber(L, e.pos.x);
			lua_pushnumber(L, e.pos.y);
			break;
		case MouseButton:
			lua_pushinteger(L, e.button.button);
			lua_pushinteger(L, e.button.action);
			lua_pushinteger(L, e.button.mods);
			break;
		case Scroll:
			lua_pushnumber(L, e.scroll.x);
			lua_pushnumber(L, e.scroll.y);
			break;
		case PathDrop:
			lua_createtable(L, static_cast<int>(e.drop.n), 0);
			for (size_t i = 0; i < e.drop.n; ++i) {
				lua_pushstring(L, e.drop.paths[i]);
				lua_seti(L, -2, static_cast<lua_Integer>(i + 1));
			}
			break;
		case FramebufferResize:
			lua_pushinteger(L, e.framebuffer.x);
			lua_pushinteger(L, e.framebuffer.y);
			break;
		case WindowClose:
			lua_pushboolean(L, e.closed);
			break;
		case WindowResize:
			lua_pushboolean(L, e.resize.x);
			lua_pushboolean(L, e.resize.y);
			break;
		case WindowMaximize:
			lua_pushboolean(L, e.maximized);
			break;
		case WindowMinimize:
			lua_pushboolean(L, e.minimized);
			break;
		case WindowFocus:
			lua_pushboolean(L, e.focused);
			break;
		}
		nargs = lua_gettop(L) - otop;
		return nargs;
	}

	template <class T, class... Args> // TODO possible api promotion
		requires std::invocable<T, Args..., EventListener&>
	void for_each_listener(lua_State* L, T f, Args&&... args) {
		get_registry_value(L, REG_LISTENERS);

		for (lua_Integer i = 0; i < seq_length(L, -1); ++i) {
			seq_get(L, -1, i + 1);
			auto p = static_cast<LuaInternalEventListener*>(lua_touserdata(L, -1));
			std::invoke(f, std::forward<Args>(args)..., *p);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	};

}

namespace foo::lua {

	EventsSubsystem::EventsSubsystem(ethread& main)
		:  L{ main.spawn() }
		, dispatch_{ nullptr } {
		get(L) = this;
		lua_createtable(L, 16, 0);
		set_registry_value(L, REG_LISTENERS);
		
		logf(SubsystemName, Logger::System, "Created {} Subsystem", SubsystemName);
	}

	EventsSubsystem::~EventsSubsystem() {
		if (dispatch_)
			for_each_listener(L, &EventDispatcher::remove_listener, dispatch_);

		lua_pushnil(L);
		set_registry_value(L, REG_LISTENERS);
		logf(SubsystemName, Logger::System, "Destroyed {} subsystem", SubsystemName);
	}

	EventsSubsystem::EventsSubsystem(EventsSubsystem&& rhs) noexcept
		: L(std::move(rhs.L))
		, dispatch_{ std::exchange(rhs.dispatch_, nullptr) } {
		get(L) = this;
	}

	EventsSubsystem& EventsSubsystem::operator=(EventsSubsystem&& rhs) noexcept {
		L = std::move(rhs.L);
		dispatch_ = std::exchange(rhs.dispatch_, nullptr);
		get(L) = this;
		return *this;
	}

	lua_Integer EventsSubsystem::create_listener() {
		stack_sentinel sentinel(L, -2 +0);
		const auto flags = static_cast<unsigned>(lua_tointeger(L, -1));
		lua_pop(L, 1);
		// F() L{}
		get_registry_value(L, REG_LISTENERS);
		const auto past_one = seq_length(L, -1) + 1;
		{
			// this block is the object where is fully constructed
			// F() L{} U
			auto mem = lua_newuserdatauv(L, sizeof(LuaInternalEventListener), 1);
			new (mem) LuaInternalEventListener(L, past_one, flags);
			lua_rotate(L, -3, -1); // L{} U F()
			lua_setiuservalue(L, -2, 1); // L{} U
		}
		lua_pushvalue(L, -1); // L{} U U
		seq_set(L, -3, past_one); // L{} U
		lua_rotate(L, -2, 1); // U L{}
		lua_pop(L, 1); // U
		debug_logf(SubsystemName, Logger::Verbose, "Created listener {}", lua_touserdata(L, -1));
		lua_pop(L, 1); // no more leaving it on top of the stack
		return past_one;
	}

	void EventsSubsystem::destroy_listener(lua_Integer id) {
		get_registry_value(L, REG_LISTENERS);
		if (id > 0 && id <= seq_length(L, -1))
			seq_unordered_remove(L, -1, id);
		lua_pop(L, 1);
	}

	void EventsSubsystem::set_dispatcher(EventDispatcher* dispatch) {
		if (dispatch_) {
			for_each_listener(L, &EventDispatcher::remove_listener, dispatch_);
		}
		if (dispatch) {
			for_each_listener(L, &EventDispatcher::add_listener, dispatch);
		}
		dispatch_ = dispatch;
		logf(SubsystemName, Logger::Info, "New dispatcher registered");
	}

}
