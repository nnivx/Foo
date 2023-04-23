#include "seq.h"

namespace foo::lua {

	void seq_unordered_remove(lua_State* L, int table, lua_Integer i) {
		// implicitly works for empty sequences though we're still wasting calls
		// unordered is also implicitly immune to relative indices
		table = lua_absindex(L, table);
		const auto last = static_cast<lua_Integer>(lua_rawlen(L, table));
		if (last != i) {
			seq_get(L, table, last); // last
			seq_set(L, table, i); // t[i] = last
		}
		lua_pushnil(L);
		seq_set(L, table, last);
	}

	void seq_remove(lua_State* L, int table, lua_Integer i) {
		// implicitly works for empty sequences though we're still wasting calls
		table = lua_absindex(L, table);
		const auto last = static_cast<lua_Integer>(lua_rawlen(L, table));
		// leq so last element is nil
		i = i < 0 ? seq_length(L, table) + i + 1 : i;
		for (; i <= last; ++i) {
			seq_get(L, table, i + 1);
			seq_set(L, table, i); // t[pos] = t[pos + 1]
		}
	}

	void seq_unordered_insert(lua_State* L, int table, lua_Integer i) {
		table = lua_absindex(L, table);
		i = i < 0 ? seq_length(L, table) + i + 2 : i;
		seq_get(L, table, i);
		seq_push(L, table);	// t[#t + 1] = t[i]
		seq_set(L, table, i); // t[i] = v
	}

	void seq_insert(lua_State* L, int table, lua_Integer i) {
		table = lua_absindex(L, table);
		auto pos = seq_length(L, table) + 1;
		i = i < 0 ? seq_length(L, table) + i + 2 : i;
		for (; pos > i; --pos) {
			seq_get(L, table, pos - 1);
			seq_set(L, table, pos); // t[pos] = t[pos - 1]
		}
		seq_set(L, table, i); // t[i] = v
	}

	int seq_at(lua_State* L, int index, lua_Integer i) {
		seq_get(L, index, i);
		const int type = lua_type(L, -1);
		lua_pop(L, 1);
		return type;
	}

}