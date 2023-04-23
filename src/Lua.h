#pragma once

#include <lua.hpp>

#include "AppRunner.h"

#include "lua_api/ethread.h"
#include "lua_api/Events.h"

#include <memory>

namespace foo {
	
	class EventDispatcher;

}

namespace foo::lua {


	class LuaContext {
	private:

		static void init_registry(lua_State* L);

	public:

		ethread L;
		EventsSubsystem events;

		EventDispatcher* dispatcher;

		LuaContext();

		//~LuaContext();

		LuaContext(const LuaContext&) = delete;
		LuaContext& operator=(const LuaContext&) = delete;

		LuaContext(LuaContext&& rhs) noexcept;
		LuaContext& operator=(LuaContext&& rhs) noexcept;

		void set_dispatcher(EventDispatcher& ndisp);

	public:

		run_config read_config(int argc, char** argv);

	};


}
#include <filesystem>

namespace foo::lua::api {

	int write_default_config(lua_State* L);


}