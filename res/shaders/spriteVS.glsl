#version 330 core

uniform mat4 mvp;
uniform vec3 col;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 texcoord;

out vec2 uv_;
out vec4 color_;

void main() {

	uv_ = texcoord;
	color_ = vec4(col, 1);
	//color_ = pos;

	gl_Position = mvp * pos;
	//gl_Position = pos;

}