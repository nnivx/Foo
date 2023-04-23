#include "_common.h"

namespace foo::lua {


	int get_registry_value(lua_State* L, std::string_view name) {
		lua_pushlstring(L, name.data(), name.length());
		return lua_rawget(L, LUA_REGISTRYINDEX);
	}

	void set_registry_value(lua_State* L, std::string_view name) {
		lua_pushlstring(L, name.data(), name.length());
		lua_pushvalue(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);
		lua_pop(L, 1);
	}

	lua_Integer inplace_arithi(lua_State* L, lua_Integer v, int op) {
		lua_pushinteger(L, v);
		lua_arith(L, op);
		return lua_tointeger(L, -1);
	}

	lua_Number inplace_arithf(lua_State* L, lua_Number v, int op) {
		lua_pushnumber(L, v);
		lua_arith(L, op);
		return lua_tonumber(L, -1);
	}

}