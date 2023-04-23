#pragma once

#include <Foo/Time.h>

#include <string>

namespace foo {

	struct run_config {
		// Window Options
		const char* window_title;
		int window_width;
		int window_height;
		bool resizable;
		bool vsync;

		// OpenGL Context
		int context_version_major;
		int context_version_minor;

		// Run Options
		int tps;
	};
	
	class Window;

	class App {
	public:

		virtual run_config config(int argc, char** argv) = 0;

		virtual void initialize(Window& window) = 0;
		virtual void cleanup() = 0;
		virtual void update(Time dt) = 0;
		virtual void render() = 0;

		virtual void integrate(Time dt) = 0;
		virtual void blend(double alpha) = 0;
		
	protected:

		virtual ~App() {

		}

	};



}