#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 color;
out vec2 texCoord;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vec4 position = vec4(aPos, 1.0);
    gl_Position = projection * view * model * position;
    texCoord = aTexCoord;
}