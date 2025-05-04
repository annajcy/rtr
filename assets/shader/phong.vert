layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;

#ifdef ENABLE_NORMAL_MAP
layout(location = 3) in vec3 a_tangent;
#endif

layout(std140, binding = 0) uniform Camera_ubo {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
    float near;
    float far;
};

uniform mat4 model;

out vec3 v_frag_position;
out vec2 v_uv;
out vec3 v_normal;

#ifdef ENABLE_NORMAL_MAP
out vec3 v_tangent;
out mat3 v_tbn;
#endif

void main() {
    gl_Position = projection * view * model * vec4(a_position, 1.0);

    v_frag_position = vec3(model * vec4(a_position, 1.0));
    v_uv = a_uv;
    v_normal = mat3(transpose(inverse(model))) * a_normal;

#ifdef ENABLE_NORMAL_MAP
    v_tangent = mat3(model) * a_tangent;
    vec3 T = normalize(v_tangent);
    vec3 N = normalize(v_normal);
    vec3 B = normalize(cross(N, T));
    v_tbn = mat3(T, B, N);
#endif

}