#include "ethread.h"

#include "_common.h"

namespace {

	using namespace foo;
	using namespace foo::lua;

	inline constexpr std::string_view RegistryID = ".threads";

	void push_registry_table(lua_State* L) {
		push_string(L, RegistryID); // TODO registry_getter push_registry_table{ RegistryID } 
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

}

namespace foo::lua {


	bool is_ethread(lua_State* L) {
		push_registry_table(L);
		const bool re = lua_istable(L, -1);
		lua_pop(L, 1);
		return re;
	}

	bool is_main_thread(lua_State* L) {
		return L == get_main_thread(L);
	}

	lua_State* get_main_thread(lua_State* L) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		auto main = lua_tothread(L, -1);
		lua_pop(L, 1);
		return main;
	}


	ethread& ethread::main(lua_State* L) {
		assert(is_ethread(L));
		push_registry_table(L);
		push_string(L, ".main");
		lua_rawget(L, -2);
		auto p = static_cast<ethread*>(lua_touserdata(L, -1));
		assert(p != nullptr);
		lua_pop(L, 2); // pop userdata and table
		return *p;
	}

	ethread::ethread()
		: L{ luaL_newstate() }
		, ref_{ LUA_NOREF } {
		// registry[.threads] = { ... ; .main = this; }
		push_string(L, RegistryID); // grep create_registry_table
		lua_createtable(L, 8, 1);
		push_string(L, ".main"); // push this for future reference
		lua_pushlightuserdata(L, this);
		lua_rawset(L, -3);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}

	ethread::~ethread() {
		if (L) { // not safe to call with nullptr
			if (is_main()) lua_close(L);
			else release_ref(L, ref_);
			// no need to destroy registry table, it closes with the state
		}
	}

	ethread::ethread(ethread&& rhs) noexcept
		: L{ std::exchange(rhs.L, nullptr) }
		, ref_{ std::exchange(rhs.ref_, LUA_REFNIL) } {

	}

	ethread& ethread::operator=(ethread&& rhs) noexcept {
		[[likely]] if (*this != rhs) {
			L = std::exchange(rhs.L, nullptr);
			ref_ = std::exchange(rhs.ref_, LUA_REFNIL);
		}
		return *this;
	}

	int ethread::acquire_ref(lua_State* L) {
		push_registry_table(L);
		lua_rotate(L, -2, 1);
		const int ref = luaL_ref(L, -2);
		lua_pop(L, 1);
		return ref;
	}

	void ethread::release_ref(lua_State* L, int ref) {
		push_registry_table(L);
		luaL_unref(L, -1, ref);
		lua_pop(L, 1);
	}


}