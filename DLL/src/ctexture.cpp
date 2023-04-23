#include <foo/ctexture.h>
#include <foo/cimage.h>
#include <foo/gltypes.h>
#include <foo/alloc.h>

#include "graphics.h"

#include <glad/glad.h>
#include <jemalloc/jemalloc.h>

#include <stdexcept>

namespace {

	int s_max_texture_size() {
		int i;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
		return i;
	}

	int s_max_texture_units() {
		int i;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i);
		return i;
	}

	void validate_size(int width, int height) {
		if (width <= 0 || height <= 0)
			throw std::runtime_error("Texture dimensions must be greater than zero.");

		if (width > max_texture_size() || height > max_texture_size())
			throw std::runtime_error("Texture too large.");
	}

	GLenum to_internal_format(GLenum sized) {
		// https://docs.gl/gl4/glTexStorage2D
		switch (sized) {
		case GL_R8:
		case GL_R8_SNORM:
		case GL_R16:
		case GL_R16_SNORM:
		case GL_R16F:
		case GL_R32F:
		case GL_R8I:
		case GL_R8UI:
		case GL_R16I:
		case GL_R16UI:
		case GL_R32I:
		case GL_R32UI:
			return GL_RED;
		case GL_RG8:
		case GL_RG8_SNORM:
		case GL_RG16:
		case GL_RG16_SNORM:
		case GL_RG16F:
		case GL_RG32F:
		case GL_RG8I:
		case GL_RG8UI:
		case GL_RG16I:
		case GL_RG16UI:
		case GL_RG32I:
		case GL_RG32UI:
			return GL_RG;
		case GL_R3_G3_B2:
		case GL_RGB4:
		case GL_RGB5:
		case GL_RGB8:
		case GL_RGB8_SNORM:
		case GL_RGB10:
		case GL_RGB12:
		case GL_RGB16_SNORM:
		case GL_RGBA2:
		case GL_RGBA4:
		case GL_SRGB8:
		case GL_RGB16F:
		case GL_RGB32F:
		case GL_R11F_G11F_B10F:
		case GL_RGB9_E5:
		case GL_RGB8I:
		case GL_RGB8UI:
		case GL_RGB16I:
		case GL_RGB16UI:
		case GL_RGB32I:
		case GL_RGB32UI:
			return GL_RGB;
		case GL_RGB5_A1:
		case GL_RGBA8:
		case GL_RGBA8_SNORM:
		case GL_RGB10_A2:
		case GL_RGB10_A2UI:
		case GL_RGBA12:
		case GL_RGBA16:
		case GL_SRGB8_ALPHA8:
		case GL_RGBA16F:
		case GL_RGBA32F:
		case GL_RGBA8I:
		case GL_RGBA8UI:
		case GL_RGBA16I:
		case GL_RGBA16UI:
		case GL_RGBA32I:
		case GL_RGBA32UI:
			return GL_RGBA;
		default:
			throw std::runtime_error("invalid sized format");
		}
	}

	GLenum to_sized_format(int components) {
		switch (components) {
		case 1: return GL_R8;
		case 2: return GL_RG8;
		case 3: return GL_RGB8;
		case 4: return GL_RGBA8;
		default:
			throw std::runtime_error("Invalid range");
		}
	}

	void check_texture_rect_bounds(const foo_texture* texture, const irect& rect) {
		new int(5);
		if (!texture->rect.contains(rect))
			throw std::runtime_error("Texture update out of bounds.");
	}

}

int max_texture_size() {
	static int v = s_max_texture_size();
	return v;
}

int max_texture_units() {
	static int v = s_max_texture_units();
	return v;
}

foo_texture* create_texture(int levels, int w, int h, int comp, bool repeat, bool filter) {
	validate_size(w, h);

	auto p = new foo_texture;
	p->id = foo::gl::texture::create(GL_TEXTURE_2D);
	p->rect = { w, h };
	p->actual_size = { w, h };
	p->format = to_internal_format(to_sized_format(comp));
	p->users = nullptr;

	glTextureStorage2D(p->id, levels, p->format, p->rect.w, p->rect.h);

	glTextureParameteri(p->id, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_BORDER);
	glTextureParameteri(p->id, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_BORDER);

	glTextureParameteri(p->id, GL_TEXTURE_MIN_FILTER, filter ? GL_LINEAR : GL_NEAREST);
	glTextureParameteri(p->id, GL_TEXTURE_MAG_FILTER, filter ? GL_LINEAR : GL_NEAREST);

	return p;
}

foo_texture* create_subtexture(const foo_texture* texture, const irect& rect) {
	check_texture_rect_bounds(texture, rect);

	auto p = new (je_malloc(sizeof(foo_texture))) foo_texture;
	p->id = texture->id;
	p->rect = rect;
	p->actual_size = { rect.w, rect.h };
	p->format = texture->format;
	p->users = texture->users = new int(2);

	return p;
}

foo_texture* load_texture(foo_image* image, bool filter) {
	auto p = create_texture(1, image->size.x, image->size.y, image->comp, false, filter);
	update_texture(p, 1, p->rect, get_image_data(image));
	return p;
}

void destroy_texture(foo_texture* texture) {
	if (!texture) return;
	if (texture->users == nullptr || --texture->users == 0) {
		foo::gl::texture::destroy(texture->id);
		delete texture->users;
	}
	delete texture;
}

void get_texture_size(const foo_texture* texture, ivec2& vec) {
	vec.x = texture->rect.w;
	vec.y = texture->rect.h;
}

void get_texture_rect(const foo_texture* texture, irect& rect) {
	rect = texture->rect;
}

void get_texture_actual_size(const foo_texture* texture, ivec2& vec) {
	vec = texture->actual_size;
}

void update_texture(foo_texture* texture, int level, const irect& rect, uint8_t* data) {
	check_texture_rect_bounds(texture, rect);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTextureSubImage2D(texture->id, level, rect.x, rect.y, rect.w, rect.h, texture->format, GL_UNSIGNED_BYTE, data);
}