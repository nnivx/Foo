#pragma once

#include <string>

#include <Foo/Time.h>


namespace foo {

	class Container;

	struct run_config {
		// Window Options
		const char* window_title;
		int window_width;
		int window_height;
		bool resizable;
		bool vsync;

		// Run Options
		int tps;

		// Path options
		std::string cwd;
	};

	class Window;

	class Application {
	protected:

		virtual ~Application() {

		}

		virtual void read_args(int argv, char** argc) {

		}

		virtual void pre_window_hook(run_config& config) {

		}

		virtual void post_window_hook(Window& window) {

		}

		virtual void initialize(Window& window) = 0;
		virtual void cleanup() = 0;
		virtual void update(Time dt) = 0;
		virtual void render() = 0;

		virtual void integrate(Time dt) = 0;
		virtual void blend(double alpha) = 0;

	private:

		friend class Container;

	};

	class Container {
	public:


	private:

	};


}