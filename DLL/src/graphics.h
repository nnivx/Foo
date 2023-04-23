#pragma once

#include <foo/config.h>
#include <foo/ctypes.h>

#include <map>

extern "C" {

	struct foo_image {
		ivec2	size;
		int		comp;
		// data after this
	};

	struct foo_globject {
		unsigned id;
	};

	struct foo_texture : foo_globject {
		irect rect;
		ivec2 actual_size;
		int format;
		mutable int* users;
	};

}
