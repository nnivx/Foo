#version 330 core

uniform vec4 col;
uniform sampler2D tex;

in vec2 uv;

out vec4 FragColor;

void main() {

	FragColor = texture2D(tex, uv) * col;

}