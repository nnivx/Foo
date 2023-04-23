#pragma once

#include <Foo/Time.h>
#include <Foo/rect.h>

#include <string_view>

#define FOO_INCLUDE_EVENT_CONSTRUCTORS 1

namespace foo {

	/// @brief Struct containing event data.
	struct Event {
	public:

		enum class Type : int {
			// Input Events
			Key,
			CharInput,
			CursorEnter,
			CursorPos,
			MouseButton,
			Scroll,

			// Drag n drop
			PathDrop,

			// Framebuffer
			FramebufferResize,

			// Window
			WindowClose,
			WindowResize,
			WindowMaximize,
			WindowMinimize,
			WindowFocus,
		};

		static inline constexpr int num_types = 13;

		struct Key {
			int key;
			int code;
			int action;
			int mods;
		};

		struct MouseButton {
			int button;
			int action;
			int mods;
		};

		struct PathDrop {
			size_t n;
			const char** paths;
		};

	public:

		Type type;					// P.S. This looks so real nice in the debugger.
		std::string_view name;
		Time timestamp;

		union {
			Key			key;
			unsigned	codept;
			bool		entered;
			glm::dvec2	pos;
			MouseButton	button;
			glm::dvec2	scroll;
			PathDrop	drop;
			glm::ivec2	framebuffer;
			bool		closed;
			glm::ivec2	resize;
			bool		maximized;
			bool		minimized;
			bool		focused;
		};

#if FOO_INCLUDE_EVENT_CONSTRUCTORS
	public:

		template <Type Tp> struct type_tag { };

		Event() noexcept = default;

		Event(type_tag<Type::Key>, int key, int code, int action, int mods) noexcept;
		Event(type_tag<Type::CharInput>, unsigned codept) noexcept;
		Event(type_tag<Type::CursorEnter>, bool entered) noexcept;
		Event(type_tag<Type::CursorPos>, double x, double y) noexcept;
		Event(type_tag<Type::MouseButton>, int button, int action, int mods) noexcept;
		Event(type_tag<Type::Scroll>, double x, double y) noexcept;
		Event(type_tag<Type::PathDrop>, size_t n, const char** paths) noexcept;
		Event(type_tag<Type::FramebufferResize>, int w, int h) noexcept;
		Event(type_tag<Type::WindowClose>, bool closed) noexcept;
		Event(type_tag<Type::WindowResize>, int w, int h) noexcept;
		Event(type_tag<Type::WindowMaximize>, bool maximized) noexcept;
		Event(type_tag<Type::WindowMinimize>, bool minimized) noexcept;
		Event(type_tag<Type::WindowFocus>, bool focused) noexcept;
#endif
	};

}
