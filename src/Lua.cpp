#include "Lua.h"
#include "luautil.h"

#include "lua_api/_common.h"
#include "lua_api/sentinel.h"
#include "lua_api/seq.h"

#include "EventListener.h"
#include "EventDispatcher.h"
#include "Logger.h"

#include <stdexcept>
#include <iostream>

#include <filesystem>

namespace {

	using namespace foo;
	using namespace foo::lua;

	inline constexpr std::string_view SubsystemName{ "LuaMain" };

	inline constexpr extraspace<LuaContext> get{ };

}

// API functions
namespace {

	template <int I, class Pred>
	inline void argcheck(lua_State* L, Pred p, const char* message) {
		if (!p(L, I)) luaL_argerror(L, I, message);
	}

	int add_listener(lua_State* L) {
		//stack_sentinel sentinel(L, -lua_gettop(L) + 1);
		// trim args
		//lua_settop(L, 2);
		luaL_argcheck(L, lua_isfunction(L, 1), 1, "expected function");
		luaL_argcheck(L, lua_isinteger(L, 2), 2, "expected integer");

		// move to subsystem thread
		lua_xmove(L, get(L)->events.thread(), 2);
		const auto id = get(L)->events.create_listener();
		lua_pushinteger(L, id);

		return 1;
	}

	int remove_listener(lua_State* L) {
		// safe to call with invalid id's
		get(L)->events.destroy_listener(lua_tointeger(L, 1));
		return 0;
	}

	int create_table(lua_State* L) {
		const int narr = lua_isnil(L, 1) ? 0 : static_cast<int>(lua_tointeger(L, 1));
		const int nrec = lua_isnil(L, 2) ? 0 : static_cast<int>(lua_tointeger(L, 2));
		lua_createtable(L, narr, nrec);
		return 1;
	}

#define APIREGENTRY(name) {#name, name}
	luaL_Reg api_reg[]{
		APIREGENTRY(add_listener),
		APIREGENTRY(remove_listener),
		APIREGENTRY(create_table),
		{nullptr, nullptr},
	};
#undef APIREGENTRY
}

namespace foo::lua {


	void LuaContext::set_dispatcher(EventDispatcher& ndisp) {
		events.set_dispatcher(&ndisp);
	}

	void LuaContext::init_registry(lua_State* L) {

	}

	static void push_path(lua_State* L, const std::filesystem::path& path) {
		const auto canonical = std::filesystem::canonical(path);
		const auto str = canonical.string();
		lua_pushlstring(L, str.c_str(), str.length());
	}

	static void init_globals(lua_State* L, LuaContext*) {
		stack_sentinel sentinel(L, 0);
		// _name = main
		lua_pushstring(L, "main");
		lua_setglobal(L, "_name");

		// _cwd = cwd 
		push_path(L, std::filesystem::current_path());
		lua_setglobal(L, "_cwd");

		// api
		luaL_newlib(L, api_reg);
		lua_setglobal(L, "api");
	}

	LuaContext::LuaContext()
		: L()
		, events(L)
		, dispatcher{ nullptr } {
		stack_sentinel sentinel(L, 0);

		get(L) = this;

		init_globals(L, this);
		luaL_openlibs(L);
		init_registry(L);

		logf(SubsystemName, Logger::System, "Created {} subsystem", SubsystemName);
		
		if (luaL_loadfile(L, "res/.scripts/main.lua") != LUA_OK) {
			std::cerr << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
			throw std::runtime_error("Error loading main script.");
		}

		if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
			std::cerr << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
			throw std::runtime_error("Exec error in main script.");
		}

		log(SubsystemName, Logger::Info, "Loaded main script.");
	}

	LuaContext::LuaContext(LuaContext&& rhs) noexcept
		: L(std::move(rhs.L))
		, events(std::move(rhs.events))
		, dispatcher{ std::exchange(rhs.dispatcher, nullptr) } {
		get(L) = this;
	}

	LuaContext& LuaContext::operator=(LuaContext&& rhs) noexcept {
		L = std::move(rhs.L);
		events = std::move(rhs.events);
		dispatcher = std::exchange(rhs.dispatcher, nullptr);
		get(L) = this;
		return *this;
	}


	static bool top_not(lua_State* L, int type) {
		if (lua_type(L, -1) == type) return false;
		pop(L);
		return true;
	}

	static int value_or(lua_State* L, int v) {
		int num = v;
		if (lua_isnumber(L, -1)) {
			int isnum;
			auto value = lua_tonumberx(L, -1, &isnum);
			if (isnum) num = static_cast<int>(value);
		}
		pop(L);
		return num;
	}

	static const char* value_or(lua_State* L, const char* v) {
		auto str = lua_isstring(L, -1) ? lua_tostring(L, -1) : v;
		pop(L);
		return str;
	}

	static bool value_or(lua_State* L, bool) {
		//auto val = lua_isboolean(L, -1) ? lua_toboolean(L, -1) : v;
		// i am bool you are bool everyone is bool
		auto val = lua_toboolean(L, -1);
		pop(L);
		return val;
	}

	template <class T>
	T get_value_or(lua_State* L, const char* field, T v) {
		table_get(L, -1, field);
		return value_or(L, v);
	}

	run_config LuaContext::read_config(int argc, char** argv) {
		run_config config{ "Untitled Window", 853, 480, false, true, 3, 3, 20 };

		if (luaL_loadfile(L, "res/.scripts/config.lua") != LUA_OK) {
			std::cerr << lua_tostring(L, -1) << std::endl;
			pop(L);
			return config;
		}

		lua_pushinteger(L, argc);

#if 0
		// TODO better table set + array set
		// +rethink push/pop interface (name, ambigous overloads etc)
		lua_createtable(L, argc, 0);
		for (int i = 0; i < argc; ++i) {
			table_rawset(L, -1, i, argv[i]);
		}
#else
		// gotchu fam
		seq_create(L, argv, argc, push_string);
#endif
		if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
			std::cerr << lua_tostring(L, -1) << std::endl;
			pop(L);
			return config;
		}

		if (top_not(L, LUA_TTABLE)) return config;

		config.window_title = get_value_or(L, "window_title", config.window_title);
		config.window_width = get_value_or(L, "window_width", config.window_width);
		config.window_height = get_value_or(L, "window_height", config.window_height);

		config.resizable = get_value_or(L, "resizable", config.resizable);
		config.vsync = get_value_or(L, "vsync", config.vsync);

		config.tps = get_value_or(L, "tps", config.tps);

		return config;
	}

}