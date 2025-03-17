#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in mat4 a_instance_matrix;

out vec3 world_position;
out vec2 uv;
out vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

void main() {
	vec4 tranform_position = vec4(a_pos, 1.0f);
	tranform_position = model_matrix * a_instance_matrix * tranform_position;
	world_position = tranform_position.xyz;
	gl_Position = projection_matrix * view_matrix * tranform_position;
	uv = a_uv;
	normal = normal_matrix * a_normal;
}