#pragma once

#include <lua.hpp>

#include <string_view>
#include <concepts>

// Internal file that also serves as staging area
// for functions to be promoted to their own file.

#ifndef NDEBUG
#include <cassert>
#endif

namespace foo::lua {

	/// @brief Push item at registry with name.
	int get_registry_value(lua_State* L, std::string_view name);

	/// @brief Pop value and set as registry item.
	void set_registry_value(lua_State* L, std::string_view name);

	/// @brief Preferred method of pushing string. 
	inline void push_string(lua_State* L, std::string_view str) {
		lua_pushlstring(L, str.data(), str.length());
	}

	/// @brief Do in-place integer op on top of the stack.
	lua_Integer inplace_arithi(lua_State* L, lua_Integer v, int op);

	/// @brief Do in-place floating-point op on top of the stack.
	lua_Number inplace_arithf(lua_State* L, lua_Number v, int op);

	/// @brief Increment value on top of the stack.
	inline lua_Integer increment(lua_State* L) {
		return inplace_arithi(L, 1, LUA_OPADD);
	}

	/// @brief Decrement value on top of the stack.
	inline lua_Integer decrement(lua_State* L) {
		return inplace_arithi(L, 1, LUA_OPSUB);
	}

	/// @brief Simple finalizer object.
	template <class T>
	struct finalizer {
		~finalizer() {
			std::invoke(func_);
		}
		T func_;
	};

	/// @brief Apply transform on table value.
	/// 
	/// Gets the field value and pushes it on the stack.
	/// Transform is called to modify the value, then whatever
	/// is left on top of the stack is put back to the field.
	template <class Func, class... Args>
		requires std::invocable<Func, lua_State*, Args...>
	auto for_field(lua_State* L, int table_index, std::string_view field, Func transform, Args&&... args) {
		table_index = lua_absindex(L, table_index);
		foo::lua::push_string(L, field);
		lua_pushvalue(L, -1);
		lua_gettable(L, table_index);
		const int otop = lua_gettop(L);
		auto set = [&]() {
			const int ntop = lua_gettop(L);
#ifndef NDEBUG
			assert(ntop >= otop);
#endif
			if (ntop > otop) {
				lua_copy(L, ntop, otop);
				lua_pop(L, ntop - otop);
			}
			lua_settable(L, table_index);
		};
		finalizer<decltype(set)> fin{ set };
		return std::invoke(transform, L, std::forward<Args>(args)...);
	}

	/// @brief Helper functor for accessing extra space.
	/// Functor so it can be aliased with using.
	template <class T>
	struct extraspace {
		inline constexpr T*& operator()(lua_State* L) const noexcept {
			return *static_cast<T**>(lua_getextraspace(L));
		}
	};


	struct typeat {
		int type;
		const char* name;

		typeat(lua_State* L, int index)
			: type{ lua_type(L, index) }
			, name{ lua_typename(L, type) } {

		}

		operator int() const noexcept {
			return type;
		}

	};

}