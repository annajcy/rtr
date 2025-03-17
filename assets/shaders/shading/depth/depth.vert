#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;

out vec2 uv;
out vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;


void main()
{
	vec4 transform_position = vec4(a_pos, 1.0);

	transform_position = model_matrix * transform_position;
	gl_Position = projection_matrix * view_matrix * transform_position;
	uv = a_uv;
	normal =  a_normal;
}