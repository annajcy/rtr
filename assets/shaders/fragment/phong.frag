#version 460 core
out vec4 frag_color;

in vec3 world_position;
in vec2 uv;
in vec3 normal;

uniform float time;
uniform sampler2D main_tex;

uniform vec3 camera_position;

struct Material {
    vec3 ka;
    vec3 kd;
    vec3 ks;
    float shininess;
}; // 添加分号

uniform Material material;

#define MAX_LIGHTS 16
uniform int active_lights;
uniform int main_light_index;

struct Light {
    int type;        // 0=ambient, 1=directional, 2=point, 3=spot
    vec3 color;
    float intensity;
    vec3 position;   // 点/聚光灯
    vec3 direction;  // 平行/聚光灯
    vec3 attenuation; // x=kc, y=k1, z=k2
    float inner_angle;
    float outer_angle;
};

uniform Light lights[MAX_LIGHTS];

// 公共光照计算函数
vec3 diffuse_component(vec3 light_dir, vec3 normal, vec3 light_color, vec3 object_color, float intensity) {
    float diff = max(dot(normal, light_dir), 0.0);
    return light_color * diff * object_color * intensity;
}

vec3 specular_component(vec3 light_dir, vec3 normal, vec3 view_dir, vec3 light_color, float shininess, float intensity) {
    // 改用半角向量计算高光
    vec3 H = normalize(light_dir + view_dir);
    float spec = pow(max(dot(normal, H), 0.0), shininess);
    return light_color * spec * intensity;
}

// 环境光处理
vec3 ambient_light(Light light, Material mat, vec3 object_color) {
    return light.color * light.intensity * mat.ka * object_color;
}

// 平行光处理
vec3 directional_light(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 object_color) {
    vec3 light_dir = normalize(-light.direction);
    vec3 diffuse = diffuse_component(light_dir, normal, light.color, object_color, mat.kd);
    vec3 specular = specular_component(light_dir, normal, view_dir, light.color, mat.shininess, mat.ks);
    return (diffuse + specular) * light.intensity;
}

// 点光源处理
vec3 point_light(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 world_pos) {
    vec3 light_dir = normalize(light.position - world_pos);
    float dist = length(light.position - world_pos);
    float attenuation = 1.0 / (light.attenuation.z * dist*dist + 
                              light.attenuation.y * dist + 
                              light.attenuation.x);
    
    vec3 diffuse = diffuse_component(light_dir, normal, light.color, mat.kd, light.intensity);
    vec3 specular = specular_component(light_dir, normal, view_dir, light.color, mat.shininess, light.intensity);
    return (diffuse + specular) * attenuation;
}

// 聚光灯处理
vec3 spot_light(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 world_pos) {
    vec3 light_dir = normalize(light.position - world_pos);
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.inner_angle - light.outer_angle;
    float intensity = clamp((theta - light.outer_angle) / epsilon, 0.0, 1.0);
    
    float dist = length(light.position - world_pos);
    float attenuation = 1.0 / (light.attenuation.z * dist*dist + 
                              light.attenuation.y * dist + 
                              light.attenuation.x);
    
    vec3 diffuse = diffuse_component(light_dir, normal, light.color, mat.kd, light.intensity);
    vec3 specular = specular_component(light_dir, normal, view_dir, light.color, mat.shininess, light.intensity);
    return (diffuse + specular) * attenuation * intensity;
}

vec3 calculate_lighting(Light light, Material mat, vec3 normal, vec3 view_dir, vec3 world_pos, vec3 object_color) {
    switch(light.type) {
        case 0: return ambient_light(light, mat, object_color);
        case 1: return directional_light(light, mat, normal, view_dir, object_color);
        case 2: return point_light(light, mat, normal, view_dir, world_pos);
        case 3: return spot_light(light, mat, normal, view_dir, world_pos);
    }
    return vec3(0.0);
}

void main() {
    vec3 object_color = texture(main_tex, uv).rgb;
    vec3 N = normalize(normal);
    vec3 V = normalize(camera_position - world_position);

    vec3 result = vec3(0.0);
    for(int i = 0; i < active_lights; ++i) {
        if(i == main_light_index) {
            continue;
        }
        result += calculate_lighting(lights[i], material, N, V, world_position, object_color);
    }
    
    if(active_lights > 0) {
        result += calculate_lighting(lights[main_light_index], mat, N, V, world_position, object_color);
    }

    frag_color = vec4(result, 1.0);
}