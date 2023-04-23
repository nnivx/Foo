#include "Window.h"

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace {

	foo::Window& get(GLFWwindow* window) {
		return *static_cast<foo::Window*>(glfwGetWindowUserPointer(window));
	}

	GLFWwindow* create_window(int width, int height, const char* title) {
		auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Failed to create GLFW Window");
		return window;
	}

	void destroy_window(GLFWwindow* window) {
		if (window)	glfwDestroyWindow(window);
	}

}

namespace foo {

	Window::Window(int width, int height, const char* title)
		: EventDispatcher(24)
		, window{ create_window(width, height, title) } {
		glfwSetWindowUserPointer(window, this);

		glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int code, int action, int mods) {
			get(w).push<Event::Type::Key>(key, code, action, mods);
			});

		glfwSetCharCallback(window, [](GLFWwindow* w, unsigned code) {
			get(w).push<Event::Type::CharInput>(code);
			});

		glfwSetCursorEnterCallback(window, [](GLFWwindow* w, int entered) {
			get(w).push<Event::Type::CursorEnter>(entered);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
			get(w).push<Event::Type::CursorPos>(x, y);
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
			get(w).push<Event::Type::MouseButton>(button, action, mods);
			});

		glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y) {
			get(w).push<Event::Type::Scroll>(x, y);
			});

		glfwSetDropCallback(window, [](GLFWwindow* w, int argc, const char** argv) {
			get(w).push<Event::Type::PathDrop>(argc, argv); // gave into the temptation lol
			});

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int width, int height) {
			get(w).push<Event::Type::FramebufferResize>(width, height);
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* w) {
			get(w).push<Event::Type::WindowClose>(true);
			});

		glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) {
			get(w).push<Event::Type::WindowResize>(width, height);
			});

		glfwSetWindowMaximizeCallback(window, [](GLFWwindow* w, int action) {
			get(w).push<Event::Type::WindowMaximize>(action);
			});

		glfwSetWindowIconifyCallback(window, [](GLFWwindow* w, int action) {
			get(w).push<Event::Type::WindowMinimize>(action);
		});

		glfwSetWindowFocusCallback(window, [](GLFWwindow* w, int action) {
			get(w).push<Event::Type::WindowFocus>(action);
			});

	}

	Window::~Window() {
		destroy_window(window);
	}

	Window::Window(Window&& rhs) noexcept
		: EventDispatcher(rhs.max_queued_events())
		, window{ std::exchange(rhs.window, nullptr) } {
		// *** very important ***
		// 
		// this was a really good bug.
		// 
		// it's an raii bug where we set the window pointer in the constructor
		// but forget to do it here, leaving a dangling WindowUserPointer set
		// set to the moved-from object that will eventually get deleted.
		// 
		// this is exactly the same problem i failed to fix like 5 years ago.
		// i like this bug because it made me more careful with raii and that
		// cases like these can bite your ass cos of hard it is to spot when
		// the pointer is nowhere in the class: it's in a 3rd party library
		// opaque pointer.
		// 
		// - nyks 2022
		//
		glfwSetWindowUserPointer(window, this);
	}

	Window& Window::operator=(Window&& rhs) noexcept {
		window = std::exchange(rhs.window, nullptr);
		// *** very important ***
		glfwSetWindowUserPointer(window, this);
		return *this;
	}

	Window::operator GLFWwindow* () {
		return window;
	}

}