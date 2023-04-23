#pragma once

#include <lua.hpp>

#include <cassert>

namespace foo::debug {

	struct stack_sentinel {

		explicit stack_sentinel(lua_State* L, int re = 0) noexcept
			: L{ L }
			, top{ lua_gettop(L) }
			, re{ re } {

		}

		~stack_sentinel() {
			const int ntop = lua_gettop(L); // for the debugger
			assert((ntop - top) == re);
		}

		operator int() const noexcept {
			return top;
		}

	private:
		const int top;
		const int re;
		lua_State* L;
	};

}