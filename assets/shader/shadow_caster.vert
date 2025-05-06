//shadow_caster.vert

layout(location = 0) in vec3 a_pos;

struct Orthographic_camera {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
    float padding;
    float near;
    float far;
    float left;
    float right;
    float bottom;
    float top;
};

layout(std140, binding = 4) uniform Light_camera_ubo {
    Orthographic_camera light_camera;
};

uniform mat4 model;

void main() {
    mat4 view = light_camera.view;
    mat4 projection = light_camera.projection;
	gl_Position = projection * view * model * vec4(a_pos, 1.0);
}