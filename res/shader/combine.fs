#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D first;
uniform sampler2D second;

void main() {
    FragColor = texture(first, TexCoords) + texture(second, TexCoords);
}