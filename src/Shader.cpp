#include <Foo/graphics/Shader.h>

#include "glhf.h"

#include <glm/gtc/type_ptr.hpp>

namespace foo {

	Shader Shader::from_source(std::string_view vert, std::string_view frag) {
		Shader shader;
		if (!gl::compile_shaders(shader.program_, vert, frag))
			throw std::runtime_error("Failed to compile shader.");
		
		//shader.get_program_uniforms();

		return shader;
	}

	Shader Shader::from_file(const fs::path& vert_path, const fs::path& frag_path) {
		auto vert = read_file(vert_path);
		auto frag = read_file(frag_path);
		return Shader::from_source(vert, frag);
	}

	//Shader::Shader(std::string_view vert, std::string_view frag) {
	//	if (!gl::compile_shaders(program_, vert, frag))
	//		throw std::runtime_error("Failed to load shader.");
	//
	//	get_program_uniforms();
	//}

	int Shader::uniform_location(const std::string& name) const {
		//return glGetProgramResourceLocation(program_, GL_UNIFORM, name.c_str());
		return glGetUniformLocation(program_, name.c_str());
	}

	void Shader::set_uniform(int location, int v0) {
		glProgramUniform1i(program_, location, v0);
	}

	void Shader::set_uniform(int location, int v0, int v1) {
		glProgramUniform2i(program_, location, v0, v1);
	}

	void Shader::set_uniform(int location, int v0, int v1, int v2) {
		glProgramUniform3i(program_, location, v0, v1, v2);
	}

	void Shader::set_uniform(int location, int v0, int v1, int v2, int v3) {
		glProgramUniform4i(program_, location, v0, v1, v2, v3);
	}

	void Shader::set_uniform(int location, float v0) {
		glProgramUniform1f(program_, location, v0);
	}

	void Shader::set_uniform(int location, float v0, float v1) {
		glProgramUniform2f(program_, location, v0, v1);
	}

	void Shader::set_uniform(int location, float v0, float v1, float v2) {
		glProgramUniform3f(program_, location, v0, v1, v2);
	}

	void Shader::set_uniform(int location, float v0, float v1, float v2, float v3) {
		glProgramUniform4f(program_, location, v0, v1, v2, v3);
	}

	void Shader::set_uniform(int location, unsigned v0) {
		glProgramUniform1ui(program_, location, v0);
	}

	void Shader::set_uniform(int location, unsigned v0, unsigned v1) {
		glProgramUniform2ui(program_, location, v0, v1);
	}

	void Shader::set_uniform(int location, unsigned v0, unsigned v1, unsigned v2) {
		glProgramUniform3ui(program_, location, v0, v1, v2);
	}

	void Shader::set_uniform(int location, unsigned v0, unsigned v1, unsigned v2, unsigned v3) {
		glProgramUniform4ui(program_, location, v0, v1, v2, v3);
	}

	void Shader::set_uniform(int location, const glm::mat4& m) {
		glProgramUniformMatrix4fv(program_, location, 1, false, glm::value_ptr(m));
	}

	void Shader::bind() const {
		glUseProgram(program_);
	}

	gl::object_id Shader::id() const {
		return program_;
	}
#if 0
	void Shader::get_program_uniforms() {
		GLint active_uniforms = 0;
		glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &active_uniforms);

		if (active_uniforms > 0) {
			GLint maxlen = 0;
			glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

			auto name = std::make_unique<char[]>(maxlen);

			for (GLint i = 0; i < active_uniforms; ++i) {
				GLsizei length = 0;
				GLint size = 0;
				GLenum type = GL_NONE;

				glGetActiveUniform(program_, i, maxlen, &length, &size, &type, name.get());

				uniforms_.try_emplace({name.get(), static_cast<size_t>(length)}, glGetUniformLocation(program_, name.get()));
			}
		}
	}
#endif
}