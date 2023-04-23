#pragma once

#include <utility>

namespace foo::gl {


	/// @brief Object handle type.
	using object_id = unsigned int;


	/// @brief Explicit tag for when we want to take ownership of a handle.
	struct take_ownership_t { } constexpr take_ownership;


	/// @brief Base OpenGL Object class.
	/// Allows type-safe and overloadable functions.
	template <class T>
	struct object {

		object(take_ownership_t, object_id id) noexcept
			: id_(id) {

		}

		object(const object&) = delete;
		object& operator=(const object&) = delete;

		object(object&& r) noexcept
			: id_(std::exchange(r.id_, 0)) {

		}

		object& operator=(object&& r) noexcept {
			id_ = std::exchange(r.id_, 0);
			return *this;
		}

		operator object_id() const noexcept {
			return id_;
		}

	protected:

		~object() {
			// safe to call with 0 and invalid names
			T::destroy(id_);
		}

	private:

		object_id id_;

	};


#ifndef FOO_USE_DSA
#define FOO_USE_DSA 0
#endif

	inline constexpr bool using_dsa = FOO_USE_DSA;

#undef FOO_USE_DSA


	/// @brief Vertex Array Object (VAO).
	class vertex_array: public object<vertex_array> {
	public:

		[[nodiscard]] static object_id create();

		static void destroy(object_id id);

		using object::object;

		vertex_array();

	};


	/// @brief Buffer Object. 
	class buffer : public object<buffer> {
	public:

		[[nodiscard]] static object_id create();

		static void destroy(object_id id);

		using object::object;

		buffer();

	};


	/// @brief Array Buffer (VBO).
	struct array_buffer : public buffer {

	};


	/// @brief Index Buffer (EBO).
	struct index_buffer : public buffer {

	};


	/// @brief Texture Object.
	class texture : public object<texture> {
	public:

		[[nodiscard]] static object_id create(unsigned target);

		static void destroy(object_id id);

		using object::object;

		texture(unsigned target);

	};


	/// @brief Texture 2D
	struct texture2D : public texture {

		texture2D();

	};


	/// @brief Framebuffer Object.
	class framebuffer : public object<framebuffer> {
	public:

		[[nodiscard]] static object_id create();

		static void destroy(object_id id);

		using object::object;

		framebuffer();

	};


	/// @brief Renderbuffer Object
	class renderbuffer : public object<renderbuffer> {
	public:

		[[nodiscard]] static object_id create();

		static void destroy(object_id id);

		using object::object;

		renderbuffer();

	};


	/// @brief Shader Object.
	class shader : public object<shader> {
	public:

		[[nodiscard]] static object_id create(unsigned type);

		static void destroy(object_id id);

		using object::object;

		shader(unsigned type);

	};


	/// @brief Vertex Shader.
	struct vertex_shader : public shader {

		vertex_shader();

	};


	/// @brief Fragment Shader.
	struct fragment_shader : public shader {

		fragment_shader();

	};


	/// @brief Shader Program Object.
	class program : public object<program> {
	public:

		[[nodiscard]] static object_id create();

		static void destroy(object_id id);

		using object::object;

		program();

	};

}