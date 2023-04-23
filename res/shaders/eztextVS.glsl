#version 330 core

uniform mat4 mat;

layout (location = 0) in vec4 coord;


out vec2 texcoord_;

void main() {

	gl_Position = mat*vec4(coord.xy, 0, 1);
	texcoord_ = coord.zw;

}