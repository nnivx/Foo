#pragma once

#include <foo/config.h>
#include <foo/ctypes.h>

extern "C" {

	struct foo_texture;
	struct foo_image;

	FOO_API int max_texture_size();

	FOO_API int max_texture_units();

	FOO_API foo_texture* create_texture(int levels, int w, int h, int comp, bool repeat, bool filter);

	FOO_API foo_texture* create_subtexture(const foo_texture* texture, const irect& rect);

	FOO_API foo_texture* load_texture(foo_image* image, bool filter);

	FOO_API void destroy_texture(const foo_texture* texture);

	FOO_API void get_texture_size(const foo_texture* texture, ivec2& vec);

	FOO_API void get_texture_rect(const foo_texture* texture, irect& rect);

	FOO_API void get_texture_actual_size(const foo_texture* texture, ivec2& vec);

	FOO_API void update_texture(foo_texture* texture, int level, const irect& rect, uint8_t* data);

}