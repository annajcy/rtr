layout(location = 0) in vec3 a_pos;

layout(std140, binding = 4) uniform Light_camera_ubo {
    mat4 light_view;
    mat4 light_projection;
    vec3 light_camera_position;
    vec3 light_camera_direction;
    float light_near;
    float light_far;
};

uniform mat4 model;

void main() {
	gl_Position = light_projection * light_view * model * vec4(a_pos, 1.0);
}