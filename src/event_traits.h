#pragma once

#include "Event.h"

#include <concepts>
#include <string_view>


namespace foo {

	/// @brief Event traits extension.
	template <Event::Type Type>
	struct event_traits;

#define EVENT_TRAITS(type_, data_) \
	template <> \
	struct event_traits<Event::Type::type_> { \
		static inline constexpr auto type = Event::Type::type_; \
		static inline constexpr std::string_view name = "Event::" # type_ ; \
		using data = data_; \
	};

	EVENT_TRAITS(Key, Event::Key)
		EVENT_TRAITS(CharInput, unsigned)
		EVENT_TRAITS(CursorEnter, bool)
		EVENT_TRAITS(CursorPos, glm::dvec2)
		EVENT_TRAITS(MouseButton, Event::MouseButton)
		EVENT_TRAITS(Scroll, glm::dvec2)
		EVENT_TRAITS(PathDrop, Event::PathDrop)
		EVENT_TRAITS(FramebufferResize, glm::ivec2)
		EVENT_TRAITS(WindowClose, bool)
		EVENT_TRAITS(WindowResize, glm::ivec2)
		EVENT_TRAITS(WindowMaximize, bool)
		EVENT_TRAITS(WindowMinimize, bool)
		EVENT_TRAITS(WindowFocus, bool)
		/*EVENT_TRAITS(Custom, Event::Custom*)*/

#undef EVENT_TRAITS

	// INSERT_NEW_EVENT_HERE <-- for grep

	/// @brief Simple identifier describing the event.
		template <Event::Type Type>
	inline constexpr auto event_name = event_traits<Type>::name;


	/// @brief The event data associated with the event type.
	template <Event::Type Type>
	using event_data = event_traits<Type>::data;


	/// @brief True if T is the event data associated with type. 
	template <class T, Event::Type Type>
	inline constexpr bool is_event_data = std::same_as<T, event_data<Type>>;


	/// @brief Concept describing event data associated with type.
	template <class T, Event::Type Type>
	concept EventData = is_event_data<T, Type>;


	// There is no event_type because there is no 1:1 mapping between
	// from event_data to event_type

	template <class>
	inline constexpr bool always_false = false;


	/// @brief Returns the associated data for event.
	template <Event::Type Type>
	constexpr const event_data<Type>& get(const Event& e) noexcept {
		static_assert(always_false<Type>, "No implementation found for get.");
	}

#ifdef NDEBUG
#define EVENT_GET(type_, name_) \
	template <> \
	constexpr const event_data<Event::Type::type_>& get<Event::Type::type_>(const Event& e) noexcept { \
		return e.name_; \
	}
#else
}

#include <cassert>
namespace foo {

#define EVENT_GET(type_, name_) \
	template <> \
	constexpr const event_data<Event::Type::type_>& get<Event::Type::type_>(const Event& e) noexcept { \
		assert(e.type == Event::Type::type_); \
		return e.name_; \
	}
#endif

	EVENT_GET(Key, key)
	EVENT_GET(CharInput, codept)
	EVENT_GET(CursorEnter, entered)
	EVENT_GET(CursorPos, pos)
	EVENT_GET(MouseButton, button)
	EVENT_GET(Scroll, scroll)
	EVENT_GET(PathDrop, drop)
	EVENT_GET(FramebufferResize, framebuffer)
	EVENT_GET(WindowClose, closed)
	EVENT_GET(WindowResize, resize)
	EVENT_GET(WindowMaximize, maximized)
	EVENT_GET(WindowMinimize, minimized)
	EVENT_GET(WindowFocus, focused)

#undef EVENT_GET


	/// @brief Returns the associated data for event.
	template <Event::Type Type>
	constexpr event_data<Type>& get(Event& e) noexcept {
		return const_cast<event_data<Type>&>(get<Type>(static_cast<const Event&>(e)));
	}


	/// @brief Constructs an event.
	template <Event::Type Type, class... Args>
		requires std::constructible_from<event_data<Type>, Args...>
	constexpr Event make_event(Args&&... args) noexcept {
		Event e;
		e.type = Type;
		get<Type>(e) = event_data<Type>(std::forward<Args>(args)...);
		e.name = event_name<Type>;
		e.timestamp = Time::now();
		return e;
	}

}