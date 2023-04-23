#pragma once

#include <lua.hpp>

namespace foo::lua {

	class ethread;


	/// @brief Returns true if a lua state is an ethread.
	bool is_ethread(lua_State* L);

	/// @brief Returns true if a lua state is the main thread.
	bool is_main_thread(lua_State* L);

	/// @brief Returns the main thread of this thread.
	lua_State* get_main_thread(lua_State* L);


	/// @brief Managed Lua execution thread.
	/// 
	/// It's a wrapped lua thread with some added functionality.
	/// One's responsible for managing the lifetimes of main and
	/// its spawned thread, otherwise undefined behaviour.
	/// 
	/// Everything in the lua_api uses this, so there shouldn't
	/// be any naked lua_State* that isn't an ethread. If it's
	/// a lua_State* it is a non-owning pointer.
	/// 
	/// ethread stores itself in the registry and doesn't make use
	/// of the extrastackspace.
	class ethread {
	public:

		static ethread& main(lua_State* L);

		ethread();
		~ethread();

		ethread(const ethread&) = delete;
		ethread& operator=(const ethread&) = delete;

		ethread(ethread&& rhs) noexcept;
		ethread& operator=(ethread&& rhs) noexcept;

	public:

		/// @brief Returns true if this thread is the main thread.
		bool is_main() const {
			return ref_ == LUA_NOREF;
		}

		/// @brief Returns the main lua thread.
		lua_State* main() {
			return get_main_thread(L);
		}

		/// @brief Returns the lua_State* held by this thread.
		lua_State* thread() {
			return L;
		}

		/// @brief Spawns a new ethread.
		ethread spawn() {
			return ethread(this->L, spawn_tag{});
		}

		/// @brief Implicitly converts to lua_State*.
		operator lua_State* () {
			return thread();
		}

		/// @brief Noexcept swap.
		friend void swap(ethread& a, ethread& b) noexcept {
			// no include std
			{ auto _tmp = a.L; a.L = b.L; b.L = _tmp; }
			{ auto _tmp = a.ref_; a.ref_ = b.ref_; b.ref_ = _tmp; }
		}

		/// @brief ethreads always have a valid lua state.
		constexpr explicit operator bool() const noexcept {
			return true;
		}

	private:

		struct spawn_tag { };

		static int acquire_ref(lua_State* L);

		static void release_ref(lua_State* L, int ref);

		ethread(lua_State* parent, spawn_tag)
			: L{ lua_newthread(parent) }
			, ref_{ acquire_ref(parent) } {

		}

	private:

		lua_State* L;
		int ref_;

	};



}