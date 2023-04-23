#pragma once

#include "App.h"

#include "Window.h"

#include <optional>

namespace foo {


	class AppRunner {
	public:

		AppRunner();

		~AppRunner();

		void operator()(App& app, int argc, char** argv);

	private:

		void set_tps(int tps);

		void read_args(int argc, char** argv);

		void main_loop(App& app);

		struct glfw_library {
			glfw_library();
			~glfw_library();
		};

		glfw_library glfw;	// !! order is important
		std::optional<Window> window;

		Time time_step_;
		int tps_;

	};

}