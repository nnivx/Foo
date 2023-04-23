#pragma once

#include <foo/config.h>
#include <foo/ctypes.h>

extern "C" {

	struct foo_shader;

	enum Shader : int {
		VertexShader,
		FragmentShader,
	};

	FOO_API unsigned	create_shader(int type);
	FOO_API bool		compile_shader(const char** src, size_t* len, int* info_log_length);
	FOO_API size_t		get_shader_info_log(char* buf, size_t bufsz);
	FOO_API void		destroy_shader(unsigned shader);

	FOO_API unsigned	create_program();
	FOO_API void		attach_shader(unsigned program, unsigned shader);
	FOO_API bool		link_program(unsigned program, int* info_log_length);
	FOO_API void		detach_shader(unsigned program, unsigned shader);
	FOO_API void		get_program_info_log(unsigned program);
	FOO_API void		destroy_program(unsigned program);

	FOO_API int 
	FOO_API int	 get_uniform_location(const foo_shader* shader);

	FOO_API void set_uniform_1i(foo_shader* shader, int loc, int v0);
	FOO_API void set_uniform_2i(foo_shader* shader, int loc, int v0, int v1);
	FOO_API void set_uniform_3i(foo_shader* shader, int loc, int v0, int v1, int v2);
	FOO_API void set_uniform_4i(foo_shader* shader, int loc, int v0, int v1, int v2, int v3);

	FOO_API void set_uniform_1f(foo_shader* shader, int loc, float v0);
	FOO_API void set_uniform_2f(foo_shader* shader, int loc, float v0, float v1);
	FOO_API void set_uniform_3f(foo_shader* shader, int loc, float v0, float v1, float v2);
	FOO_API void set_uniform_4f(foo_shader* shader, int loc, float v0, float v1, float v2, float v3);

	FOO_API void set_uniform_1u(foo_shader* shader, int loc, unsigned v0);
	FOO_API void set_uniform_2u(foo_shader* shader, int loc, unsigned v0, unsigned v1);
	FOO_API void set_uniform_3u(foo_shader* shader, int loc, unsigned v0, unsigned v1, unsigned v2);
	FOO_API void set_uniform_4u(foo_shader* shader, int loc, unsigned v0, unsigned v1, unsigned v2, unsigned v3);

	FOO_API void set_uniform_mat3(foo_shader* shader, int loc, const mat3& mat);
	FOO_API void set_uniform_mat4(foo_shader* shader, int loc, const mat4& mat);

}