#pragma once

#include <Foo/graphics/gltypes.h>

#include <glad/glad.h>

#include <string>
#include <string_view>
#include <filesystem>

namespace fs = std::filesystem;

namespace foo {

	std::string read_file(const fs::path& path);

}

namespace foo::gl {

	const char* error_string(GLenum err) noexcept;

	//void check_error(const char* file, int line);

	GLenum to_internal_format(GLenum sized);

	GLenum to_sized_format(int components);

	bool compile_shader(gl::shader& s, std::string_view source);

	bool compile_shaders(gl::program& p, std::string_view vert_source, std::string_view frag_source);

	bool load_shaders(gl::program& p, std::filesystem::path vert_path, std::filesystem::path frag_path);

}