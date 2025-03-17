#version 460 core
layout (location = 0) in vec3 a_pos;

out vec3 uvw;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;


void main()
{
	vec4 transform_position = vec4(a_pos, 1.0);
	transform_position = model_matrix * transform_position;
	vec4 pos = projection_matrix * view_matrix * transform_position;
	gl_Position = pos.xyww;
	uvw = a_pos;
}