#include "Event.h"

#if FOO_INCLUDE_EVENT_CONSTRUCTORS
#include "event_traits.h"

namespace foo {

	Event::Event(Event::type_tag<Type::Key>, int key, int code, int action, int mods) noexcept
		: type{ Type::Key }
		, key{ key, code, action, mods }
		, name{ event_name<Type::Key> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::CharInput>, unsigned codept) noexcept
		: type{ Type::CharInput }
		, codept{ codept }
		, name{ event_name<Type::CharInput> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::CursorEnter>, bool entered) noexcept
		: type{ Type::CursorEnter }
		, entered{ entered }
		, name{ event_name<Type::CursorEnter> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::CursorPos>, double x, double y) noexcept
		: type{ Type::CursorPos }
		, pos{ x, y }
		, name{ event_name<Type::CursorPos> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::MouseButton>, int button, int action, int mods) noexcept
		: type{ Type::MouseButton }
		, button{ button, action, mods }
		, name{ event_name<Type::MouseButton> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::Scroll>, double x, double y) noexcept
		: type{ Type::Scroll }
		, scroll{ x, y }
		, name{ event_name<Type::Scroll> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::PathDrop>, size_t n, const char** paths) noexcept
		: type{ Type::PathDrop }
		, drop{ n, paths }
		, name{ event_name<Type::PathDrop> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::FramebufferResize>, int w, int h) noexcept
		: type{ Type::FramebufferResize }
		, framebuffer{ w, h }
		, name{ event_name<Type::FramebufferResize> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::WindowClose>, bool closed) noexcept
		: type{ Type::WindowClose }
		, closed{ closed }
		, name{ event_name<Type::WindowClose> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::WindowResize>, int w, int h) noexcept
		: type{ Type::WindowResize }
		, resize{ w, h }
		, name{ event_name<Type::WindowResize> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::WindowMaximize>, bool maximized) noexcept
		: type{ Type::WindowMaximize }
		, maximized{ maximized }
		, name{ event_name<Type::WindowMaximize> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::WindowMinimize>, bool minimized) noexcept
		: type{ Type::WindowMinimize }
		, minimized{ minimized }
		, name{ event_name<Type::WindowMinimize> }
		, timestamp{ Time::now() } {

	}

	Event::Event(Event::type_tag<Type::WindowFocus>, bool focused) noexcept
		: type{ Type::WindowFocus }
		, focused{ focused }
		, name{ event_name<Type::WindowFocus> }
		, timestamp{ Time::now() } {

	}

}
#endif