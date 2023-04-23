#pragma once

#include <Foo/graphics/Image.h>
#include <Foo/graphics/Texture.h>
#include <Foo/graphics/Shader.h>

namespace foo {

	// A debug drawable that's proven and guaranteed to work.
	class Plane {
	public:

		Plane(float z);

		void draw(const glm::mat4& mvp);

	private:

		Image image;
		Texture texture;
		Shader shader;

		gl::vertex_array vao;
		gl::buffer vbo;

		int loc_mvp;

	};

}