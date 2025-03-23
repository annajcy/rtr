#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec3 world_position;
out vec2 uv;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
	vec4 tranform_position = vec4(a_pos, 1.0f);
	tranform_position = model_matrix * tranform_position;
	gl_Position = projection_matrix * view_matrix * tranform_position;
	
	world_position = tranform_position.xyz;
	uv = a_uv;
}