#include <foo/gltypes.h>

#include <glad/glad.h>


namespace foo::gl {

	static_assert(std::is_same<object_id, GLuint>::value, "object_id != GLuint");
	static_assert(std::is_same<unsigned, GLenum>::value, "unsigned != GLenum");

	// TODO non-dsa code should return a default-created object like DSA does

	//////////////////////////////////////////////////////////////////
	// VAO
	//////////////////////////////////////////////////////////////////

	object_id vertex_array::create() {
		object_id id;
		if constexpr (using_dsa) {
			glCreateVertexArrays(1, &id);
		}
		else {
			glGenVertexArrays(1, &id);
		}
		return id;
	}

	void vertex_array::destroy(object_id id) {
		glDeleteVertexArrays(1, &id);
	}

	vertex_array::vertex_array()
		: object(take_ownership, create()) {

	}

	//////////////////////////////////////////////////////////////////
	// VBO
	//////////////////////////////////////////////////////////////////

	object_id buffer::create() {
		object_id id;
		if constexpr (using_dsa) {
			glCreateBuffers(1, &id);
		}
		else {
			glGenBuffers(1, &id);
		}
		return id;
	}

	void buffer::destroy(object_id id) {
		glDeleteBuffers(1, &id);
	}

	buffer::buffer()
		: object(take_ownership, create()) {

	}

	//////////////////////////////////////////////////////////////////
	// Texture
	//////////////////////////////////////////////////////////////////

	object_id texture::create(unsigned target) {
		object_id id;
		if constexpr (using_dsa) {
			glCreateTextures(target, 1, &id);
		}
		else {
			glGenTextures(1, &id);
		}
		return id;
	}

	void texture::destroy(object_id id) {
		glDeleteTextures(1, &id);
	}

	texture::texture(unsigned target)
		: object(take_ownership, create(target)) {

	}

	texture2D::texture2D()
		: texture(GL_TEXTURE_2D) {

	}

	//////////////////////////////////////////////////////////////////
	// Framebuffer
	//////////////////////////////////////////////////////////////////

	object_id framebuffer::create() {
		object_id id;
		if constexpr (using_dsa) {
			glCreateFramebuffers(1, &id);
		}
		else {
			glGenFramebuffers(1, &id);
		}
		return id;
	}

	void framebuffer::destroy(object_id id) {
		glDeleteFramebuffers(1, &id);
	}

	framebuffer::framebuffer()
		: object(take_ownership, create()) {

	}

	//////////////////////////////////////////////////////////////////
	// Renderbuffer
	//////////////////////////////////////////////////////////////////

	object_id renderbuffer::create() {
		object_id id;
		if constexpr (using_dsa) {
			glCreateRenderbuffers(1, &id);
		}
		else {
			glGenRenderbuffers(1, &id);
		}
		return id;
	}

	void renderbuffer::destroy(object_id id) {
		glDeleteRenderbuffers(1, &id);
	}

	renderbuffer::renderbuffer()
		: object(take_ownership, create()) {

	}

	//////////////////////////////////////////////////////////////////
	// Shader
	//////////////////////////////////////////////////////////////////

	object_id shader::create(unsigned type) {
		return glCreateShader(type);
	}

	void shader::destroy(object_id id) {
		glDeleteShader(id);
	}

	shader::shader(unsigned type)
		: object(take_ownership, create(type)) {

	}

	vertex_shader::vertex_shader()
		: shader(GL_VERTEX_SHADER) {

	}

	fragment_shader::fragment_shader()
		: shader(GL_FRAGMENT_SHADER) {

	}

	//////////////////////////////////////////////////////////////////
	// Program
	//////////////////////////////////////////////////////////////////

	object_id program::create() {
		return glCreateProgram();
	}

	void program::destroy(object_id id) {
		glDeleteProgram(id);
	}

	program::program()
		: object(take_ownership, create()) {

	}

}