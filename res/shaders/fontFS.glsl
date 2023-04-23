#version 330 core

uniform sampler2D tex;
uniform vec3 color;

out vec4 FragColor;

in vec2 texcoord_;

void main() {
	// aren't we wasting texture space by using it this way tho?
	// what about the g and b components that are == r
	FragColor = vec4(1, 1, 1, texture(tex, texcoord_).r) * vec4(color, 1);
	//FragColor = vec4(1, 1, 1, 1) * vec4(color, 1);
}