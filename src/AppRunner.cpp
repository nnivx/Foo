#include "AppRunner.h"

#include "Logger.h"
#include "Clock.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <format>
#include <iostream>
#include <memory>

namespace {

	using namespace foo;

	Window create_window(const run_config& cfg) {
		log("Container", Logger::Info, "creating window");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, cfg.context_version_major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, cfg.context_version_minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, cfg.resizable);

		Window window{ cfg.window_width, cfg.window_height, cfg.window_title };

		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			throw std::runtime_error("Failed to load GLAD");
		}

		if (cfg.vsync)
			glfwSwapInterval(1);

		return window;
	}

	int g_users = 0;

}

namespace foo {

	AppRunner::glfw_library::glfw_library() {
		if (g_users == 0) {
			if (!glfwInit())
				throw std::runtime_error("Failed to initialize GLFW");

			log("Container", Logger::Info, "GLFW initialized");
		}
		++g_users;
	}

	AppRunner::glfw_library::~glfw_library() {
		--g_users;
		if (g_users == 0) {
			glfwTerminate();
			log("Container", Logger::Info, "GLFW terminated");
		}
	}

	AppRunner::AppRunner()
		: time_step_{}
		, tps_{} {
		glfwSetErrorCallback([](int err, const char* msg) {
				logf("Container", Logger::Error, "GLFW Error {}: {}", err, msg);
			});
	}

	AppRunner::~AppRunner() {
	}

	void AppRunner::operator()(App& app, int argc, char** argv) {
		log("Container", Logger::Verbose, "Reading args");
		read_args(argc, argv);

		log("Container", Logger::Verbose, "Reading config");
		const auto cfg = app.config(argc, argv);

		log("Container", Logger::Verbose, "Creating window");
		// FIX: window cannot die before the game
		window = create_window(cfg);

		set_tps(cfg.tps);

		log("Container", Logger::Verbose, "Initializing game");
		app.initialize(*window);
		try {
			log("Container", Logger::Verbose, "Running main loop");
			main_loop(app);
		}
		catch (const std::runtime_error& e) {
			logf("Container", Logger::Error, "Exception caught in main loop: {}", e.what());
		}
		log("Container", Logger::Verbose, "Cleaning up game");
		app.cleanup();
	}

	void AppRunner::set_tps(int tps) {
		time_step_ = seconds(1.0 / tps);
		tps_ = tps;
	}

	void AppRunner::read_args(int, char**) {
		// do nothing
	}

	void AppRunner::main_loop(App& app) {
		// https://gafferongames.com/post/fix_your_timestep
		auto accumulator = Time::zero();
		Clock clock;

		while (!glfwWindowShouldClose(*window)) {
			// process window events
			glfwPollEvents();
			window->update();

			auto frame_time = clock.restart();
			if (frame_time > 0.25_sec)
				frame_time = 0.25_sec;

			accumulator += frame_time;

			// per-frame update
			app.update(frame_time);

			// integrate sim with fixed dt steps
			while (accumulator >= time_step_) {
				app.integrate(time_step_);
				accumulator -= time_step_;
			}

			// blend two states together
			app.blend(accumulator / time_step_);

			// render
			app.render();

			// swap and poll events
			glfwSwapBuffers(*window);
		}
	}

}