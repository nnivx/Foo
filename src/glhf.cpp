#include "glhf.h"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector> // TODO used for getting compile logs

namespace foo {

	std::string read_file(const fs::path& path) {
		auto in = std::ifstream(path);
		if (!in) {
			auto s = path.string();
			throw std::runtime_error(std::format("Cannot read from '{}'", s.c_str()));
		}

		return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

}

namespace foo::gl {

	const char* error_string(GLenum err) noexcept {
		switch (err) {
		case GL_NO_ERROR:			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:		return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:		return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:	return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW:		return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:	return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:		return "GL_OUT_OF_MEMORY";
			//case GL_TABLE_TOO_LARGE:
			//	return "GL_TABLE_TOO_LARGE";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:
			return "unknown error";
		}
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

	bool compile_shader(gl::shader& s, std::string_view source) {
		{
			const char* src[1]{ source.data() };
			GLint len[1]{ static_cast<GLint>(source.length()) };
			glShaderSource(s, 1, src, len);
		}
		glCompileShader(s);

		int status;
		glGetShaderiv(s, GL_COMPILE_STATUS, &status);
		if (!status) {
			int len = 0;
			glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);

			std::vector<char> buf(len);
			glGetShaderInfoLog(s, len, nullptr, buf.data());
			std::cerr << std::string_view(buf.data(), len) << std::endl;
		}
		return status;
	}

	bool compile_shaders(gl::program& p, std::string_view vert_source, std::string_view frag_source) {
		gl::shader v(GL_VERTEX_SHADER);
		if (!compile_shader(v, vert_source))
			return false;

		gl::shader f(GL_FRAGMENT_SHADER);
		if (!compile_shader(f, frag_source))
			return false;

		glAttachShader(p, v);
		glAttachShader(p, f);
		glLinkProgram(p);
		glDetachShader(p, v);
		glDetachShader(p, f);

		int success;
		glGetProgramiv(p, GL_LINK_STATUS, &success);
		if (!success) {
			int len = 0;
			glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);

			std::vector<char> buf(len);
			glGetProgramInfoLog(p, len, nullptr, buf.data());
			std::cerr << std::string_view(buf.data(), len) << std::endl;
		}
		return success;
	}

	bool load_shaders(gl::program& p, std::filesystem::path vert_path, std::filesystem::path frag_path) {
		auto vs = read_file(vert_path);
		auto fs = read_file(frag_path);

		return compile_shaders(p, vs, fs);
	}

}