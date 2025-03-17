#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform float time;
uniform float speed;

void main()
{
    float offsetX = sin(time * speed) * 0.3;
    gl_Position = vec4(aPos.x + offsetX, aPos.y, aPos.z, 1.0);
    color = aColor * (sin(time * speed) * 0.5 + 0.5);
}