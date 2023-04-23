#version 330 core

uniform sampler2D tex;

out vec4 FragColor;

in vec2 uv_;
in vec4 color_;

void main() {
	FragColor = texture(tex, uv_) * color_;
	//FragColor = color_;
}