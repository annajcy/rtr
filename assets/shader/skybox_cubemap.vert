// skybox_cubemap.vert

layout(location = 0) in vec3 a_position;

layout(std140, binding = 0) uniform Camera_ubo {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

out vec3 v_uvw;

void main() {
    mat4 model = mat4(1.0);
    model[3] = vec4(camera_position, 1.0); // 直接设置第四列
    gl_Position = (projection * view * model * vec4(a_position, 1.0)).xyww;
    v_uvw = a_position;
}