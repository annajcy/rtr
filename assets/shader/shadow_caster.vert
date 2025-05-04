#define CAMERA_COUNT 10

layout(location = 0) in vec3 a_pos;

struct Camera {
    mat4 light_view;
    mat4 light_projection;
    vec3 light_camera_position;
    vec3 light_camera_direction;
    float light_near;
    float light_far;
};

layout(std140, binding = 4) uniform Light_camera_ubo {
    int count;
    Camera light_cameras[CAMERA_COUNT];
};

uniform mat4 model;
uniform int invocation_id;

void main() {
    mat4 light_projection = light_cameras[invocation_id].light_projection;
    mat4 light_view = light_cameras[invocation_id].light_view;
	gl_Position = light_projection * light_view * model * vec4(a_pos, 1.0);
}