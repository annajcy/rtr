#version 330 core

out vec4 FragColor;
in vec3 color;
in vec2 texCoord;

uniform vec2 resolution;
uniform sampler2D textureSampler0;

void main() {

    // vec2 location = texCoord * resolution;
    // vec2 dx = dFdx(location);
    // vec2 dy = dFdy(location);

    // float maxDelta = sqrt(max(dot(dx, dx), dot(dy, dy)));
    // int level = max(0, int(log2(maxDelta) + 0.5));

    FragColor = texture(textureSampler0, texCoord);
}