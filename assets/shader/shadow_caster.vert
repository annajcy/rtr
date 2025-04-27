layout(location = 0) in vec3 a_pos;

layout(std140, binding = 0) uniform Light_matrix_ubo {
    mat4 light_matrix;
};

uniform mat4 model;

void main() {
	gl_Position = light_matrix * model * vec4(a_pos, 1.0);
}