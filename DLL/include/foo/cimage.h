#pragma once

#include <foo/config.h>
#include <foo/ctypes.h>

extern "C" {

	struct foo_image;

	FOO_API int valid_image_size(int size);

	FOO_API foo_image* create_image(int w, int h, int comp);

	FOO_API foo_image* load_image(const char* path, int* comp, bool flip, const char** err);

	FOO_API void destroy_image(foo_image* image);

	FOO_API void get_image_size(const foo_image* image, ivec2& vec);

	FOO_API int get_image_components(const foo_image* image);

	FOO_API uint8_t* get_image_data(foo_image* image);

	FOO_API bool save_image(foo_image* image, const char* path);

}