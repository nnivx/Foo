#version 330 core

uniform sampler2D mask;
uniform vec4 color;

in vec2 texcoord_;

out vec4 FragColor;

void main() {

	FragColor = vec4(1, 1, 1, texture(mask, texcoord_).r) * color;
	//FragColor = vec4(0,0,0,1);
}