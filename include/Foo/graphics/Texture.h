#pragma once

#include <Foo/rect.h>
#include <Foo/graphics/gltypes.h>

// TODO make importers not have to include all these headers
#include <glad/glad.h>

#include <memory>

namespace foo {

	class Image;


	/// @brief Simple Texture2D class.
	class Texture {
	public:

		static constexpr inline bool power_of_two_textures = false;

		/// @brief Extra texture creation parameters.
		struct ExtraParams {
			GLenum wrap_s;
			GLenum wrap_t;
			GLenum min_filter;
			GLenum mag_filter;
		};

		/// @brief Default Texture parameters
		static constexpr ExtraParams Default{
			GL_CLAMP_TO_EDGE,
			GL_CLAMP_TO_EDGE,
			GL_LINEAR,
			GL_LINEAR,
		};

		/// @brief Returns maximum texture size.
		static int max_size();

		/// @brief Returns maximum number of active textures.
		static int max_units();

	public:

		static Texture from_image(const Image& image, const ExtraParams& params = Default);
		
		Texture(const Texture& source, const irect& rect);

		Texture(int levels, int width, int height, GLenum sized_format, const ExtraParams& params = Default);

	public:

		void wrap(GLenum s, GLenum t);

		void filter(GLenum min, GLenum mag);

		void update(int level, const frect& rect, const uint8_t* pixels);

		void bind(GLuint unit = 0) const;

		GLenum format() const;

		irect rect() const;

		glm::ivec2 size() const;

		int width() const;

		int height() const;

		glm::ivec2 actual_size() const;

		gl::object_id id() const;

	private:

		struct Shared {
			gl::texture texture;
			GLenum		format;
			glm::ivec2	actual_size;
		};

		static std::shared_ptr<Shared> make_shared(int levels, int width, int height, GLenum sized_format);

		std::shared_ptr<Shared> ptr_;
		irect rect_;

	};

	glm::fvec2 normalize_uv(int u, int v, int w, int h);

	glm::fvec2 normalize_uv(const Texture& tex, const glm::ivec2& uv);

	//Texture pack_atlas();

}