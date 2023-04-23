#pragma once

#include <lua.hpp>

#include <iterator>

// use raw functions on release or if explicitly told not to
#if !defined(FOO_SEQ_ALLOW_METAMETHODS) && NDEBUG
#define FOO_SEQ_USE_RAW_FUNCTIONS 1 
#else
#define FOO_SEQ_USE_RAW_FUNCTIONS 0
#endif

namespace foo::lua {


	/// @brief Whether we're using raw functions.
	inline constexpr bool seq_use_raw_functions = FOO_SEQ_USE_RAW_FUNCTIONS;


	/// Sequences can have non-integer keys, as long as
	/// there are no holes in the positive integers.


	/// @brief Pushes t[i] for seq at the given index.
	inline int seq_get(lua_State* L, int index, lua_Integer i) {
		if constexpr (seq_use_raw_functions) {
			return lua_rawgeti(L, index, i);
		}
		else {
			return lua_geti(L, index, i);
		}
	}


	/// @brief Pops value on the top of the stack and sets it
	/// to t[i] for seq at the given index.
	inline void seq_set(lua_State* L, int table, lua_Integer i) {
		if constexpr (seq_use_raw_functions) {
			lua_rawseti(L, table, i);
		}
		else {
			lua_seti(L, table, i);
		}
	}


	/// @brief Returns the length of the sequence at index.
	inline lua_Integer seq_length(lua_State* L, int index) {
		if constexpr (seq_use_raw_functions) {
			// potentially unsafe: check lua wraparound mechanics
			return static_cast<lua_Integer>(lua_rawlen(L, index));
		}
		else {
			lua_len(L, index);
			lua_Integer v = lua_tointeger(L, -1);
			lua_pop(L, 1);
			return v;
		}
	}


	/// @brief Creates a seq and pushes it onto the stack.
	template <std::input_iterator Iter, class Pusher>
	inline void seq_create(lua_State* L, Iter begin, Iter end, Pusher push) {
		lua_createtable(L, static_cast<int>(std::distance(begin, end)), 0);
		for (size_t i = 1; begin != end; ++begin, ++i) {
			push(L, *begin);
			seq_set(L, -2, static_cast<lua_Integer>(i));
		}
	}

	/// @brief Creates a seq and pushes it onto the stack.
	template <class T, class Pusher>
	inline void seq_create(lua_State* L, const T* p, size_t n, Pusher push) {
		lua_createtable(L, static_cast<int>(n), 0);
		for (size_t i = 0; i < n; ++i) {
			push(L, *p);
			++p;
			seq_set(L, -2, static_cast<lua_Integer>(i + 1));
		}
	}


	/// @brief Creates a seq and pushes it onto the stack.
	inline void seq_create(lua_State* L, size_t n, size_t extra_fields = 0) {
		lua_createtable(L, static_cast<int>(n), static_cast<int>(extra_fields));
	}


	/// @brief Pops value at top of the stack and pushes it onto seq.
	inline void seq_push(lua_State* L, int index) {
		seq_set(L, index, seq_length(L, index) + 1);
	}


	/// @brief Pops last element of seq.
	inline void seq_pop(lua_State* L, int index) {
		lua_pushnil(L);
		seq_set(L, index, seq_length(L, index));
	}


	/// @brief Removes item t[i] of seq at index. Might shuffle contents.
	void seq_unordered_remove(lua_State* L, int index, lua_Integer i);


	/// @brief Removes item t[i] of seq at index.
	void seq_remove(lua_State* L, int index, lua_Integer i);


	/// @brief Inserts item at [i] of seq at index. Might shuffle contents.
	void seq_unordered_insert(lua_State* L, int index, lua_Integer i);


	/// @brief Inserts item at [i] of seq at index.
	void seq_insert(lua_State* L, int index, lua_Integer i);


	/// @brief Returns the type of item at [i] of seq at index.
	int seq_at(lua_State* L, int index, lua_Integer i);


}
