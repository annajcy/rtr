#version 330 core

out vec4 FragColor;
in vec3 color;
in vec2 texCoord;

uniform vec2 resolution;
uniform sampler2D textureSampler0;

void main() {
    FragColor = texture(textureSampler0, texCoord);
}