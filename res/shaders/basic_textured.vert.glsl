#version 330 core

uniform mat4 mvp;
uniform vec4 col;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoord;

out vec2 texcoord_;
out vec4 color_;

void main() {
	texcoord_ = texcoord;
	color_ = col;
	gl_Position = mvp*vec4(pos, 1);
}