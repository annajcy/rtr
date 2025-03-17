#version 460 core
out vec4 frag_color;

in vec2 uv;
in vec3 normal;


void main()
{
	frag_color = vec4(vec3(1.0,1.0,1.0), 1.0);
}