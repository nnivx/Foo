#pragma once

#include "EventDispatcher.h"

struct GLFWwindow;

namespace foo {

	/// @brief Very simple RAII wrapper for GLFWwindow
	class Window: public EventDispatcher {
	public:

		Window(int width, int height, const char* title);

		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&& rhs) noexcept;
		Window& operator=(Window&& rhs) noexcept;

	public:

		operator GLFWwindow* ();

	private:

		GLFWwindow* window;

	};

}