#include <iostream>

#include <Foo/graphics/gltypes.h>
#include <Foo/rect.h>

#include "App.h"
#include "AppRunner.h"

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <glfw/glfw3.h>

#include <stack>
#include <optional>
#include <iostream>

#define CURRENT_TESTING
#ifdef CURRENT_TESTING
#include <Foo/graphics/Texture.h>
#include <Foo/graphics/Image.h>
#include <Foo/graphics/Shader.h>

#include "SpriteRenderer.h"
#include "Sprite.h"
#include "Camera.h"

#endif

#include <list>
#include <vector>
#include <format>

#include <glm/gtc/type_ptr.hpp>

#if !NDEBUG
#define check(f) do { f; check_error(__FILE__, __LINE__); } while(0)
#include <format>
#include "glhf.h"
#else
#define check(f) f
#endif
static void check_error(const char* file, int line) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::string s = std::format("OpenGL Error: {} ({}) [{}] {}", file, line, err, foo::gl::error_string(err));
		std::cerr << s << std::endl;
	}
}

#include "Plane.h"
#include "event_traits.h"
#include "EventDispatcher.h"
#include "EventListener.h"
#include "Window.h"
#include "Lua.h"

#include <stack>
#include <bit>


namespace foo::gui {

	struct base;
	struct root;
	struct inputfield;
	struct mousearea;
	struct screen;
	struct label;
	struct image;

	struct base {
		struct node {
			base* next;
			base* child;
			int users;
		};
		node node;

		int layer;
		irect area;
	};

	template <int Tp>
	struct typed_base : base {
		static constexpr int type = Tp;
	};

	struct root : base {
		inputfield* input;
	};

	struct mousearea : base {

	};

	struct inputfield : base {

	};

	struct screen : base {
		enum : unsigned {
			Transparent = 1 << 0,
			Flat		= 1 << 1,
			Resizable	= 1 << 3,
			KeepAspect	= 1 << 4,
		};

		unsigned flags;
		float alpha;
		glm::fvec3 color;
	};

	struct label : screen {
		std::string text;
		int font;
	};

	struct image : screen {
		int texture;
	};



}

namespace foo::game {

	struct RenderContextInfo {
		void* render_target;
		void* user;

		glm::fvec4 clear_color;
		struct {
			unsigned depth : 1;
			unsigned stencil : 1;
			unsigned scissor : 1;
		} flags;

		irect viewport;
		irect scissor;

		Camera camera;

		Window* window;
	};

	struct AutoContext {

		explicit AutoContext(std::stack<RenderContextInfo>& context)
			: context_{ &context } {
			context_->push(context_->top());
			bind();
		}

		~AutoContext() {
			context_->pop();
		}

		void bind() const {
			const auto& c = context_->top();

			glViewport(c.viewport.x, c.viewport.y, c.viewport.w, c.viewport.h);
			if (c.flags.scissor) {
				glEnable(GL_SCISSOR_TEST);
				glScissor(c.scissor.x, c.scissor.y, c.scissor.w, c.scissor.h);
			}

			GLbitfield clear_mask = GL_COLOR_BUFFER_BIT;
			if (c.flags.depth) {
				glEnable(GL_DEPTH_TEST);
				clear_mask |= GL_DEPTH_BUFFER_BIT;
			}
			if (c.flags.stencil) {
				glEnable(GL_STENCIL_TEST);
				clear_mask |= GL_STENCIL_BUFFER_BIT;
			}
			glClearColor(c.clear_color.x, c.clear_color.y, c.clear_color.z, c.clear_color.w);
			glClear(clear_mask);
		}

		RenderContextInfo& operator*() {
			return context_->top();
		}

		RenderContextInfo* operator->() {
			return &context_->top();
		}

	private:
		std::stack<RenderContextInfo>* context_;
	};

	class EZGame : public App, EventListener {
	public:

		unsigned event_type() const noexcept override {
			return ~0U;
		}

		void operator()(const Event& e) override {
			if (e.type == Event::Type::WindowClose) {
				std::cout << "Closing, goodbye\n";
			}

			if (e.type == Event::Type::FramebufferResize) {
				framebuffer_resize(e.framebuffer.x, e.framebuffer.y);
			}
		}

		std::stack<RenderContextInfo> context;
		lua::LuaContext lua;


		struct GameData {
			std::optional<Image> test_image;
			std::optional<Texture> texture;
			std::optional<Texture> texture2;
			std::optional<SpriteRenderer> renderer;
			Plane plane1 = -1;
			Plane plane2 = 1;
			Sprite* sprite = nullptr;
		};
		std::unique_ptr<GameData> gd;

		void init_game() {
			// TODO next asset manager
			// lua manager
			// more robust camera + worldspace scaling and inverse view
			check(gd = std::make_unique<GameData>());
			check(gd->test_image = Image::load("res\\images\\square.jpeg", 3, true));
			if (!gd->test_image)
				throw std::runtime_error("cannot load test image");
			check(gd->texture = Texture::from_image(*gd->test_image));

			gd->texture2 = Texture::from_image(*Image::load("res/images/source.jpg", 3, true));

			gd->renderer = std::make_optional<SpriteRenderer>(15);

			gd->sprite = gd->renderer->create(&*gd->texture);

			gd->renderer->create(&*gd->texture2);
		}

		void cleanup_game() {
			gd = nullptr;
		}

		void framebuffer_resize(int w, int h) {
			std::cout << "Resized " << w << 'x' << h << '\n';

			auto& top = context.top();
			top.viewport = irect(0, 0, w, h);
			top.scissor = irect(0, 0, w, h);

			top.camera.position({ 0, 0, 3 })
				.look_at({ 0, 0, 0 })
#if 1
				.ortho(w, h, -100, 100);
#else
				.perspective(glm::radians(45.f), w, h, 0.1f, 100);
#endif
		}

		run_config config(int argc, char** argv) override {
			auto cfg = lua.read_config(argc, argv);
			if constexpr (gl::using_dsa) {
				// TODO why is this even a config option, this should be hardcoded right
				cfg.context_version_major = 4;
				cfg.context_version_minor = 5;
			}
			return cfg;
		}

		void init_window(Window& window) {
			lua.set_dispatcher(window);

			int w, h;
			glfwGetWindowSize(window, &w, &h);
			framebuffer_resize(w, h);

			window.add_listener(*this);
		}

		void init_context(Window& window) {
			RenderContextInfo info;
			info.flags.scissor = true;
			info.window = &window;
			info.clear_color = { 0.3f, 0.3f, 0.3f, 0.3f };
			context.push(info);
		}

		void initialize(Window& window) override {
			check(init_context(window));
			check(init_window(window));
			check(init_game());
		}

		void cleanup() override {
			context.top().window->remove_listener(*this);
			cleanup_game();
		}

		void update(Time) override {

		}

		void render() override {
			AutoContext ctx(context);

			gd->renderer->update();

			gd->renderer->begin_draw(ctx->camera.matrix());

			gd->renderer->draw();

		}

		
		void integrate(Time) override {
		}

		void blend(double) override {

		}

	};

}

#define TEST 0

#include "Lua.h"

#include "lua_api/_common.h"
#include "lua_api/seq.h"
#include "lua_api/sentinel.h"
using namespace foo::lua;

lua_Integer seq_absindex(lua_State* L, int table, lua_Integer i) {
	return i < 0 ? seq_length(L, table) + i + 1 : i;
}

void print_field(lua_State* L, std::string_view field) {
	foo::lua::push_string(L, field);
	lua_rawget(L, 1);
	std::cout << field << ": " << lua_tostring(L, -1) << std::endl;
	lua_pop(L, 1);
}

void print_field(lua_State* L, lua_Integer field) {
	lua_rawgeti(L, 1, field);
	std::cout << field << ": " << lua_tostring(L, -1) << std::endl;
	lua_pop(L, 1);
}

// Have all important system functions here
#include "Logger.h" 
// Buffer, allocator, string pool, etc

#include <iostream>
#include <fstream>

#if 0
/*
	LuaMain
		main execution thread
		events

*/
#endif


// TODO 
template <class F>
	requires std::invocable<F, lua_State*, lua_Integer>
inline void seq_enumerate(lua_State* L, int index, F func) {
	const int top = lua_gettop(L);
	for (lua_Integer i = 1; i <= seq_length(L, index); ++i) {
		// push item on top
		seq_get(L, index, i);
		// call function
		func(L, i);
		const int ntop = lua_gettop(L);
		// we got extra, remove them
		if (ntop > top) {
			lua_settop(L, top);
		}
#if !NDEBUG
		// we got less and stack is disturbed
		else if (ntop < top) throw "stack underflow";
#endif
		}
	}

#include <thread>
#include <future>
#include "Clock.h"
int main(int argc, char** argv) {
	// Create the global logger.
	std::ostream* out = &std::cout;
	std::ostream* err = &std::cerr;
#if NDEBUG
	std::fstream logs_out("logs/log.txt", std::fstream::out);
	if (out) out = &logs_out;

	std::fstream logs_err("logs/errlog.txt", std::fstream::out);
	if (err) err = &logs_err;
#endif
	foo::Logger logger{ foo::Logger::All, *out, *err };
	foo::set_logger(logger);

#if TEST
	foo::Clock timer;
	auto file = std::async([]() {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		return 5;
		});
	std::cout << timer.restart().seconds() << std::endl;


	//std::thread thr([]() {
	//	std::cout << "in thread " << std::this_thread::get_id() << '\n';
	//	});
	//
	//std::cout << "in main " << std::this_thread::get_id() << '\n';

	
	auto L = ethread(); 
	{
		foo::lua::stack_sentinel sentinel(L);
		lua_createtable(L, 0, 1);

		lua_pushstring(L, "count");
		lua_pushinteger(L, 5);
		lua_rawset(L, -3);

		lua_pushstring(L, "count");
		lua_pushvalue(L, -1);
		lua_rawget(L, -3);
		std::cout << lua_tointeger(L, -1) << std::endl;
		auto i = foo::lua::decrement(L);
		std::cout << "i: " << i << "\tv: " << lua_tointeger(L, -1) << std::endl;
		lua_rawset(L, -3);


		print_field(L, "count");

		foo::lua::for_field(L, -1, "count", foo::lua::decrement);

		print_field(L, "count");

		// check flexibility
		auto v = for_field(L, -1, "please", lua_pushstring, "yo");
		//for_field(L, -1, "please", lua_pushinteger, 1);

		print_field(L, "please");

		lua_settop(L, sentinel);
	}

	//thr.join();

	int vs[]{ 1, 1, 2, 3, 5, 8, 13, 21 };
	foo::lua::seq_create(L, vs, sizeof(vs) / sizeof(int), lua_pushinteger);
	lua_Integer i = 0;
	for (auto p = std::begin(vs); p != std::end(vs); ++p) {
		print_field(L, ++i);
	}
	seq_enumerate(L, -1, [](lua_State* L, lua_Integer i) {
		std::cout << lua_tointeger(L, -1) << std::endl;
		lua_pop(L, 1);
		});

	auto remove_func = seq_remove;
	auto remove_then_print = [&](lua_Integer i) {
		std::cout << "tookening away\n";
		remove_func(L, -1, i);
		for (auto z = 1; z <= lua_rawlen(L, -1); ++z) {
			print_field(L, z);
		}
	};

	std::cout << "insertening away\n";
	lua_pushinteger(L, 50);
	seq_unordered_insert(L, -2, 1);
	for (auto z = 1; z <= lua_rawlen(L, -1); ++z) {
		print_field(L, z);
	}

	remove_then_print(-1);
	remove_then_print(-3);
	remove_then_print(-2);
	remove_then_print(-1);
	remove_then_print(-1);
	remove_then_print(-4);

	std::cout << "done" << std::endl;


	std::cout << timer.restart().seconds() << std::endl;

	std::cout << file.get() << " : " << timer.restart().seconds() << std::endl;
#else
	// constructor initializes most basic utilities like
	// Logger, Allocator, and Library inits
	foo::AppRunner run;

	// Consruct game in the heap
	auto game = std::make_unique<foo::game::EZGame>();

	// Run game
	run(*game, argc, argv);
#endif
	return 0;
}