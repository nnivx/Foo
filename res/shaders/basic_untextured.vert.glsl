#version 330 core

uniform mat4 mvp;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoord;

out vec2 uv;

void main() {

	uv = texcoord;
	gl_Position = mvp * vec4(pos, 1);
}