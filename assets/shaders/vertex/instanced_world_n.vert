#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;

// 添加uniform实例矩阵数组
#define MAX_INSTANCES 100
uniform mat4 instance_matrices[MAX_INSTANCES];

out vec3 world_position;
out vec2 uv;
out vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
    // 使用gl_InstanceID获取当前实例索引
    mat4 instance_matrix = instance_matrices[gl_InstanceID];
    
    vec4 transform_position = model_matrix * instance_matrix * vec4(a_pos, 1.0);
    world_position = transform_position.xyz;
    gl_Position = projection_matrix * view_matrix * transform_position;
    
    // 修正法线变换矩阵计算
    mat3 normal_matrix = transpose(inverse(mat3(model_matrix * instance_matrix)));
    normal = normal_matrix * a_normal;
    
    uv = a_uv;
}