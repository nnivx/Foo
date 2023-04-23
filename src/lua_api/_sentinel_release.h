#pragma once

#include <lua.hpp>

namespace foo::release {

	struct stack_sentinel {

		explicit stack_sentinel(lua_State* L, int = 0) noexcept
			: top{ lua_gettop(L) } {

		}

		operator int() const noexcept {
			return top;
		}

	private:
		const int top;
	};

}