#include "Plane.h"

#include <glm/vec4.hpp>

namespace foo {


	Plane::Plane(float z)
		: image(*Image::load("res/images/source.jpg", 3, true))
		, texture(Texture::from_image(image))
		, shader(Shader::from_file("res/shaders/basic_untextured.vert.glsl", "res/shaders/basic_untextured.frag.glsl")) {
		const float pos[]{
			-0.5,  0.5, z, 0, 1,
			-0.5, -0.5, z, 0, 0,
		 	 0.5,  0.5, z, 1, 1,
			 0.5, -0.5, z, 1, 0,
		};

		const glm::fvec4 col{ 1, 1, 1, 1 };

		loc_mvp = shader.uniform_location("mvp");
		shader.set_uniform(glGetUniformLocation(shader.id(), "col"), col);
		shader.set_uniform(glGetUniformLocation(shader.id(), "tex"), 0);

		glNamedBufferStorage(vbo, sizeof(pos), pos, GL_DYNAMIC_STORAGE_BIT);

		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);

		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
		glVertexArrayAttribBinding(vao, 1, 0);

		glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(float));

		glNamedBufferSubData(vbo, 0, sizeof(pos), pos);
	}

	void Plane::draw(const glm::mat4& mvp) {
		shader.bind();
		glBindVertexArray(vao);
		texture.bind(0);

		//const auto mvp = glm::scale(context.camera.matrix(), glm::fvec3(200, 200, 200));
		shader.set_uniform(loc_mvp, mvp);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}


}