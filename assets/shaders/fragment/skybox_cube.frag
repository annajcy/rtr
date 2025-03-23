#version 460 core
out vec4 frag_color;

in vec3 uvw;

uniform samplerCube cube_map_sampler;

void main()
{
	frag_color = texture(cube_map_sampler, uvw);
}