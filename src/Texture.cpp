#include <Foo/graphics/Texture.h>
#include <Foo/graphics/Image.h>

#include "glhf.h"

namespace {

	using namespace foo;

	int max_texture_size() {
		int i;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
		return i;
	}

	int max_texture_units() {
		int i;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i);
		return i;
	}

	void validate_size(int width, int height) {
		if (width <= 0 || height <= 0)
			throw std::runtime_error("Texture dimensions must be greater than zero.");

		if (width > Texture::max_size() || height > Texture::max_size())
			throw std::runtime_error("Texture too large.");
	}

}

namespace foo {


	int Texture::max_size() {
		static int v = max_texture_size();
		return v;
	}

	int Texture::max_units() {
		static int v = max_texture_units();
		return v;
	}

	Texture Texture::from_image(const Image& image, const ExtraParams& params) {
		Texture t{
			1,
			static_cast<int>(image.width()),
			static_cast<int>(image.height()),
			gl::to_sized_format(image.components()),
			params
		};
		t.update(0, { 1, 1 }, image.data());
		return t;
	}

	Texture::Texture(const Texture& source, const irect& rect)
		: ptr_(source.ptr_)
		, rect_(rect) {
		if (!source.rect_.contains(rect))
			throw std::runtime_error("Texture view out of range.");
	}

	Texture::Texture(int levels, int width, int height, GLenum sized_format, const ExtraParams& params)
		: ptr_{ make_shared(levels, width, height, sized_format) }
		, rect_{ 0, 0, width, height } {

		glTextureStorage2D(id(), levels, sized_format, width, height);

		wrap(params.wrap_s, params.wrap_t);
		filter(params.min_filter, params.mag_filter);
	}

	void Texture::wrap(GLenum s, GLenum t) {
		glTextureParameteri(id(), GL_TEXTURE_WRAP_S, s);
		glTextureParameteri(id(), GL_TEXTURE_WRAP_T, t);
	}

	void Texture::filter(GLenum min, GLenum mag) {
		glTextureParameteri(id(), GL_TEXTURE_MIN_FILTER, min);
		glTextureParameteri(id(), GL_TEXTURE_MAG_FILTER, mag);
	}

	void Texture::Texture::update(int level, const frect& rect, const uint8_t* pixels) {
		const auto r = expand(rect, size());
		if (!rect_.contains(r))
			throw std::runtime_error("Texture update out of bounds.");

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(id(), level, r.x, r.y, r.w, r.h, ptr_->format, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture::bind(GLuint unit) const {
		glBindTextureUnit(unit, id());
	}

	GLenum Texture::format() const {
		return ptr_->format;
	}

	irect Texture::rect() const {
		return rect_;
	}

	glm::ivec2 Texture::size() const {
		return { rect_.w, rect_.h };
	}

	int Texture::width() const {
		return rect_.w;
	}

	int Texture::height() const {
		return rect_.h;
	}

	glm::ivec2 Texture::actual_size() const {
		return ptr_->actual_size;
	}

	gl::object_id Texture::id() const {
		return ptr_->texture;
	}

	std::shared_ptr<Texture::Shared> Texture::make_shared(int levels, int width, int height, GLenum sized_format) {
		if (levels <= 0 || levels > 4)
			throw std::runtime_error("Levels out of range.");

		validate_size(width, height);

		if constexpr (power_of_two_textures) {
			// might overflow, but GPU prob gonna die first with that size
			width = std::bit_ceil(static_cast<unsigned>(width));
			height = std::bit_ceil(static_cast<unsigned>(height));

			validate_size(width, height);
		}

		const auto internal_format = gl::to_internal_format(sized_format);

		return std::make_shared<Texture::Shared>(GL_TEXTURE_2D, internal_format, glm::ivec2(width, height));
	}


	glm::fvec2 normalize_uv(int u, int v, int w, int h) {
		return { (u + 0.5f) / w, (v + 0.5f) / h };
	}

	glm::fvec2 normalize_uv(const Texture& tex, const glm::ivec2& uv) {
		const auto r = tex.rect();
		const auto a = tex.actual_size();
		return normalize_uv(r.x + uv.x, r.y + uv.y, a.x, a.y);
	}

}