#include <foo/cimage.h>

#include "graphics.h"

#include <jemalloc/jemalloc.h>

#define STBI_NO_GIF

#define STBI_ASSERT(x) static_cast<void*>(0)
#define STBI_MALLOC je_malloc
#define STBI_REALLOC je_realloc
#define STBI_FREE je_free

#define STB_IMAGE_IMPLEMENTATION
#pragma warning (push, 0)
#pragma warning( disable: 26451 )
#include "stb_image.h"
#pragma warning (pop)

#include <algorithm>

namespace {

	static inline constexpr int max_image_size = (1 << 24) / 1000 * 2;

}

static_assert(std::same_as<uint8_t, stbi_uc>, "Incompatible data types.");

int valid_image_size(int size) {
	return std::clamp(size, 0, max_image_size);
}

foo_image* create_image(int w, int h, int comp) {
	w = valid_image_size(w);
	h = valid_image_size(h);
	comp = std::clamp(comp, 1, 4);

	const size_t sz = sizeof(foo_image) + static_cast<size_t>(w) + static_cast<size_t>(h) + static_cast<size_t>(comp);
	void* packedmem = je_aligned_alloc(alignof(foo_image), sz);
	return new (packedmem) foo_image{{w, h}, comp };
}

foo_image* load_image(const char* path, int* comp, bool flip, const char** err) {
	stbi_set_flip_vertically_on_load(flip);

	int x, y, n;
	if (stbi_info(path, &x, &y, &n)) {
		const int want_comp = comp? *comp: n;
		auto data = stbi_load(path, &x, &y, comp, want_comp);
		if (data) {
			auto img = create_image(x, y, want_comp);

			// we're doing an extra copy here just to have
			// locality for the image and its metadata lol
			const size_t sz = static_cast<size_t>(x) + static_cast<size_t>(y) + static_cast<size_t>(want_comp);

			std::uninitialized_copy(data, data + sz, get_image_data(img));
			stbi_image_free(data);

			return img;
		}
	}
	if (err) *err = stbi_failure_reason();
	return nullptr;
}

void destroy_image(foo_image* image) {
	if (image) je_free(image);
}

void get_image_size(const foo_image* image, ivec2& vec) {
	vec = image->size;
}

int get_image_components(const foo_image* image) {
	return image->comp;
}

uint8_t* get_image_data(foo_image* image) {
	return reinterpret_cast<uint8_t*>(image) + sizeof(foo_image);
}

bool save_image(foo_image*, const char*) {
	return false;
}
