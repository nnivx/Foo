#pragma once

#if 0
	void function_that_pops_one(lua_State* L) {
		// put this at the beginning of the scope
		stack_sentinel sentinel(L, -1 +1);
		
		// push ... etc
		
		// implicity converts to top
		const int top = sentinel;

		// breaks if we dont leave the stack with the
		// promised return condition (-1 in this case)
	}
#endif

// For debug builds this triggers a breakpoint if the
// condition isn't met. In release code it simply as
// a reference point for top() at construction.

#ifndef NDEBUG
#include "_sentinel_debug.h"
namespace foo::lua { using debug::stack_sentinel; }
#else
#include "_sentinel_release.h"
namespace foo::lua { using release::stack_sentinel; }
#endif