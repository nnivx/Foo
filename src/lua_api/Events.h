#pragma once

#include "ethread.h"

#include <string_view>

namespace foo {

	struct Event;
	class EventListener;
	class EventDispatcher;

}

namespace foo::lua {

	// Events Subsystem.
	// Has its own independent execution branch.
	class EventsSubsystem {
	public:

		EventsSubsystem(ethread& main);

		~EventsSubsystem();

		EventsSubsystem(const EventsSubsystem&) = delete;
		EventsSubsystem& operator=(const EventsSubsystem&) = delete;

		EventsSubsystem(EventsSubsystem&&) noexcept;
		EventsSubsystem& operator=(EventsSubsystem&&) noexcept;

	public:

		/// @brief Takes the function and type from the top of the stackand creates a listener.
		/// Returns the id of the created listener.
		lua_Integer create_listener();

		/// @brief Destroys listener. Does nothing if id is invalid.
		void destroy_listener(lua_Integer id);

		/// @brief Register listeners to dispatcher.
		/// This also unregisters from the old dispatcher.
		/// Can be called with null to remove listeners.
		/// 
		/// TODO this exists cos of the current design where we run
		/// the main script but the window isnt active yet. however
		/// the api exposes our functions but at that time the
		/// dispatcher havent been made yet.
		/// A proper boot sequence is required to make this work.
		void set_dispatcher(EventDispatcher* dispatch);

	private:

		ethread L;
		EventDispatcher* dispatch_;

	};

}