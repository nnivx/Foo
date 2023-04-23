#pragma once

#include <lua.hpp>

#include <string_view>

namespace foo::lua {

    struct global_table_tag { };
    struct this_thread_tag { };

    constexpr inline global_table_tag global_table{ };
    constexpr inline this_thread_tag this_thread{ };

    inline void push(lua_State* L, bool b) {
        lua_pushboolean(L, b);
    }

    inline void push(lua_State* L, lua_CFunction fn, int n = 0) {
        lua_pushcclosure(L, fn, n);
    }

    template <class... Args>
    inline void push(lua_State* L, const char* fmt, Args&&... args) {
        lua_pushfstring(L, fmt, std::forward<Args>(args)...);
    }

    inline void push(lua_State* L, global_table_tag) {
        lua_pushglobaltable(L);
    }

    inline void push(lua_State* L, lua_Integer n) {
        lua_pushinteger(L, n);
    }

    inline void push(lua_State* L, void* p) {
        lua_pushlightuserdata(L, p);
    }

    inline void push(lua_State* L, const char* s, size_t len) {
        lua_pushlstring(L, s, len);
    }

    inline void push(lua_State* L, nullptr_t) {
        lua_pushnil(L);
    }

    inline void push(lua_State* L, lua_Number n) {
        lua_pushnumber(L, n);
    }

    inline void push(lua_State* L, std::string_view s) {
        lua_pushlstring(L, s.data(), s.length());
    }

    inline void push(lua_State* L, this_thread_tag) {
        lua_pushthread(L);
    }

    inline void dup(lua_State* L, int i = -1) {
        lua_pushvalue(L, i);
    }

    inline void pop(lua_State* L, int n = 1) {
        lua_pop(L, n);
    }


    /// @brief Set t[k] = v without metamethods.
    /// @param index - index of the table in the stack.
    template <class K, class V> // TODO
    inline void table_rawset(lua_State* L, int index, K k, const V v) {
        const int idx = lua_absindex(L, index);
        push(L, k);
        push(L, v);
        lua_rawset(L, idx);
    }


    /// @brief Set t[i] = v without metamethods.
    /// @param index - index of the table in the stack.
    template <class V>
    inline void table_rawset(lua_State* L, int index, std::integral auto i, const V v) {
        const int idx = lua_absindex(L, index);
        push(L, v);
        lua_rawseti(L, idx, i);
    }


    /// @brief Set t[k] = v.
    /// @param index - index of the table in the stack.
    template <class K, class V>
    inline void table_set(lua_State* L, int index, K&& k, V&& v) {
        const int idx = lua_absindex(L, index);
        push(L, std::forward<K>(k));
        push(L, std::forward<V>(v));
        lua_settable(L, idx);
    }


    /// @brief Set t[i] = v.
    /// @param index - index of the table in the stack.
    template <class V>
    inline void table_set(lua_State* L, int index, lua_Integer i, V&& v) {
        const int idx = lua_absindex(L, index);
        push(L, std::forward<V>(v));
        lua_seti(L, idx, v);
    }


    /// @brief Set t.field = v.
    /// @param index - index of the table in the stack.
    template <class V>
    inline void table_set(lua_State* L, int index, const char* field, V&& v) {
        const int idx = lua_absindex(L, index);
        push(L, std::forward<V>(v));
        lua_setfield(L, idx, field);
    }


    /// @brief Pushes t[k] onto the stack without metamethods.
    /// @param index - index of the table in the stack.
    template <class K>
    inline int table_rawget(lua_State* L, int index, K&& k) {
        const int idx = lua_absindex(L, index);
        push(L, std::forward<K>(k));
        return lua_rawget(L, idx);
    }


    /// @brief Pushes t[i] onto the stack without metamethods.
    /// @param index - index of the table in the stack.
    inline int table_rawget(lua_State* L, int index, lua_Integer i) {
        return lua_rawgeti(L, index, i);
    }


    /// @brief Pushes t[k] onto the stack.
    /// @param index - index of the table in the stack.
    template <class K>
    inline int table_get(lua_State* L, int index, K&& k) {
        const int idx = lua_absindex(L, index);
        push(L, std::forward<K>(k));
        return lua_gettable(L, idx);
    }


    /// @brief Pushes t[i] onto the stack.
    /// @param index - index of the table in the stack.
    inline int table_get(lua_State* L, int index, lua_Integer i) {
        return lua_geti(L, index, i);
    }


    /// @brief Pushes t.field onto the stack.
    /// @param index - index of the table in the stack.
    inline int table_get(lua_State* L, int index, const char* field) {
        return lua_getfield(L, index, field);
    }

}