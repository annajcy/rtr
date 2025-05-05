
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define PI_HALF 1.5707963267948966
#define PI_QUARTER 0.7853981633974483

#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8

in vec2 v_uv;
in vec3 v_frag_position;
in vec3 v_normal;

out vec4 frag_color;

struct Camera {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
    float near;
    float far;
};

layout(std140, binding = 0) uniform Camera_ubo {
    Camera main_camera;
};

struct Directional_light {
    float intensity;
    vec3 color;
    vec3 direction;
};

layout(std140, binding = 1) uniform Directional_light_array_ubo {
    int dl_count;
    Directional_light dl_lights[MAX_DIRECTIONAL_LIGHT];
};

struct Point_light {
    float intensity;
    vec3 position;
    vec3 color;
    vec3 attenuation;
};

layout(std140, binding = 2) uniform Point_light_array_ubo {
    int pl_count;
    Point_light pl_lights[MAX_POINT_LIGHT];
};

struct Spot_light {
    float intensity;
    float inner_angle_cos;
    float outer_angle_cos;
    vec3 direction;
    vec3 position;
    vec3 color;
};

layout(std140, binding = 3) uniform Spot_light_array_ubo {
    int spl_count;
    Spot_light spl_lights[MAX_SPOT_LIGHT];
};

vec3 calculate_diffuse(
    vec3 normal, 
    vec3 light_dir, 
    float intensity, 
    vec3 light_color, 
    vec3 albedo,
    vec3 kd
) {
    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    return kd * intensity * light_color * diffuse_factor * albedo;
}

vec3 calculate_specular(
    vec3 normal, 
    vec3 view_dir, 
    vec3 light_dir, 
    float intensity, 
    vec3 light_color, 
    vec3 albedo, 
    vec3 spec_mask,
    float shininess,
    vec3 ks
) {
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_factor = pow(max(dot(normal, halfway_dir), 0.0), shininess);
    return ks * intensity * light_color * specular_factor * albedo * spec_mask;
}

float calculate_spot_intensity(vec3 light_dir, vec3 spot_dir, float inner_cos, float outer_cos) {
    float theta = dot(-light_dir, normalize(spot_dir));
    float epsilon = inner_cos - outer_cos;
    return clamp((theta - outer_cos) / epsilon, 0.0, 1.0);
}

uniform float transparency;
uniform vec3 ka;     
uniform vec3 kd;     
uniform vec3 ks;    
uniform float shininess;  

// 纹理采样器
#ifdef ENABLE_ALBEDO_MAP
layout(binding = 0) uniform sampler2D albedo_map;
#endif

#ifdef ENABLE_SPECULAR_MAP
layout(binding = 1) uniform sampler2D specular_map;
#endif

#ifdef ENABLE_NORMAL_MAP
in vec3 v_tangent;
in mat3 v_tbn;
layout(binding = 2) uniform sampler2D normal_map;
#endif

#ifdef ENABLE_ALPHA_MAP
layout(binding = 3) uniform sampler2D alpha_map;
#endif

#ifdef ENABLE_HEIGHT_MAP
layout(binding = 4) uniform sampler2D height_map;

uniform float parallax_scale;
uniform float parallax_layer_count;

vec2 parallax_uv(vec2 uv, vec3 view_dir, sampler2D height_map, mat3 tbn, float scale) {
	view_dir = normalize(transpose(tbn) * view_dir);
    view_dir.xy /= -view_dir.z;

    vec2 offset = view_dir.xy * texture(height_map, uv).r * scale;
    return uv + offset;
}

vec2 parallax_steep_uv(vec2 uv, vec3 view_dir, sampler2D height_map, mat3 tbn, float scale, float layer_count) {
	view_dir = normalize(transpose(tbn) * view_dir);
    view_dir.xy /= -view_dir.z;

    float layer_depth = 1.0 / layer_count;
    vec2 delta_uv = view_dir.xy * scale / layer_count;

    vec2 current_uv = uv;
    float current_height = texture(height_map, current_uv).r;
    float current_depth = 0.0;

    while (current_height > current_depth) {
        current_uv += delta_uv;
        current_height = texture(height_map, current_uv).r;
        current_depth += layer_depth;
    }

    return current_uv;
}

vec2 parallax_occlusion_uv(vec2 uv, vec3 view_dir, sampler2D height_map, mat3 tbn, float scale, float layer_count) {
	view_dir = normalize(transpose(tbn) * view_dir);
    view_dir.xy /= -view_dir.z;

    float layer_depth = 1.0 / layer_count;
    vec2 delta_uv = view_dir.xy * scale / layer_count;

    vec2 current_uv = uv;
    float current_height = texture(height_map, current_uv).r;
    float current_depth = 0.0;

    while (current_height > current_depth) {
        current_uv += delta_uv;
        current_height = texture(height_map, current_uv).r;
        current_depth += layer_depth;
    }

    vec2 prev_uv = current_uv - delta_uv;
    float prev_height = texture(height_map, prev_uv).r;
    float prev_depth = current_depth - layer_depth;
    float prev_distance = prev_height - prev_depth;
    float current_distance = current_depth - current_height;

    float weight = current_distance / (current_distance + prev_distance);
    return mix(current_uv, prev_uv, weight);
}
#endif

#ifdef ENABLE_SHADOWS

#define MAX_PCF_SAMPLE 32

layout(binding = 5) uniform sampler2D dl_shadow_map;

struct Orthographic_camera {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
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

uniform float shadow_bias;
uniform float light_size;
uniform float pcf_radius;
uniform float pcf_tightness;
uniform int pcf_sample_count;

float rand_2to1(vec2 uv) { 
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

void generate_poisson_disk_samples( // 更正函数名
    vec2 uv_seed,
    int pcf_sample_count,
    float pcf_tightness,
    out vec2 samples[MAX_PCF_SAMPLE]
) {
    float angle = rand_2to1(uv_seed) * PI2;
    float radius = 1.0 / float(pcf_sample_count);
    float radius_increment = radius;
    float angle_increment = (1.0 - 0.6180339887) * PI2;

    pcf_sample_count = min(pcf_sample_count, MAX_PCF_SAMPLE); // 限制样本数量

    for (int i = 0; i < pcf_sample_count; i++) {
        float x = cos(angle) * pow(radius, pcf_tightness);
        float y = sin(angle) * pow(radius, pcf_tightness);
        samples[i] = vec2(x, y);
        angle += angle_increment;
        radius += radius_increment;
    }
}

float pcf(
    vec3 projected_position,
    float pcf_radius,
    float pcf_tightness,
    int pcf_sample_count,
    float bias,
    sampler2D shadow_map
) {
    vec2 uv = projected_position.xy;
    float frag_depth = projected_position.z;

    vec2 poisson_disk_samples[MAX_PCF_SAMPLE]; // 更正变量名
    generate_poisson_disk_samples(
        uv,
        pcf_sample_count,
        pcf_tightness,
        poisson_disk_samples
    );

    float sum = 0.0;
    pcf_sample_count = min(pcf_sample_count, MAX_PCF_SAMPLE);
    for (int i = 0; i < pcf_sample_count; i++) {
        float shadow_map_depth = texture(shadow_map, uv + poisson_disk_samples[i] * pcf_radius).r;
        sum += (frag_depth - bias > shadow_map_depth) ? 1.0 : 0.0;
    }
    return sum / float(pcf_sample_count);
}

#endif

void main() {
    vec3 camera_position = main_camera.camera_position;
    vec3 camera_direction = main_camera.camera_direction;


    vec3 view_direction = normalize(camera_position - v_frag_position);

#ifdef ENABLE_HEIGHT_MAP
    vec2 uv = parallax_occlusion_uv(
        v_uv, 
        -view_direction, 
        height_map, 
        v_tbn, 
        parallax_scale,
        parallax_layer_count
    );
#else
    vec2 uv = v_uv;
#endif

#ifdef ENABLE_ALPHA_MAP
    float alpha = texture(alpha_map, uv).r * transparency;
#else
    float alpha = transparency;
#endif

#ifdef ENABLE_NORMAL_MAP
    vec3 normal_map_normal = texture(normal_map, uv).rgb * 2.0 - vec3(1.0);
    vec3 normal = normalize(v_tbn * normal_map_normal);
    vec3 normalized_normal = normalize(normal);
#else
    vec3 normalized_normal = normalize(v_normal);
#endif

#ifdef ENABLE_ALBEDO_MAP
    vec4 albedo = texture(albedo_map, uv);
#else
    vec4 albedo = vec4((normalized_normal + 1.0) / 2.0, 1.0);
#endif

#ifdef ENABLE_SPECULAR_MAP
    vec3 specular_mask = texture(specular_map, uv).rrr;
#else
    vec3 specular_mask = vec3(1.0);
#endif

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    ambient += ka * albedo.rgb;

    // 计算方向光
    for (int i = 0; i < dl_count; i++) {
        vec3 light_dir = normalize(-dl_lights[i].direction);
        
        diffuse += calculate_diffuse(
            normalized_normal, 
            light_dir, 
            dl_lights[i].intensity, 
            dl_lights[i].color, 
            albedo.rgb,
            kd
        );

        specular += calculate_specular(
            normalized_normal, 
            view_direction, 
            light_dir, 
            dl_lights[i].intensity, 
            dl_lights[i].color, 
            albedo.rgb, 
            specular_mask,
            shininess,
            ks
        );
    }

    // 计算点光源
    for (int i = 0; i < pl_count; i++) {
        vec3 light_dir = normalize(pl_lights[i].position - v_frag_position);
        float distance = length(pl_lights[i].position - v_frag_position);
        float attenuation = 1.0 / 
            (pl_lights[i].attenuation.x + 
            pl_lights[i].attenuation.y * distance + 
            pl_lights[i].attenuation.z * distance * distance);
        
        diffuse += calculate_diffuse(
            normalized_normal, 
            light_dir, 
            pl_lights[i].intensity, 
            pl_lights[i].color, 
            albedo.rgb,
            kd
        ) * attenuation;

        specular += calculate_specular(
            normalized_normal, 
            view_direction, 
            light_dir, 
            pl_lights[i].intensity, 
            pl_lights[i].color, 
            albedo.rgb, 
            specular_mask,
            shininess,
            ks
        ) * attenuation;
    }

    // 计算聚光灯
    for (int i = 0; i < spl_count; i++) {
        vec3 light_dir = normalize(spl_lights[i].position - v_frag_position);
        float intensity = calculate_spot_intensity(
            light_dir, 
            spl_lights[i].direction, 
            spl_lights[i].inner_angle_cos, 
            spl_lights[i].outer_angle_cos
        );
        
        diffuse += calculate_diffuse(
            normalized_normal, 
            light_dir, 
            spl_lights[i].intensity, 
            spl_lights[i].color, 
            albedo.rgb,
            kd
        ) * intensity;

        specular += calculate_specular(
            normalized_normal, 
            view_direction, 
            light_dir, 
            spl_lights[i].intensity, 
            spl_lights[i].color, 
            albedo.rgb, 
            specular_mask,
            shininess,
            ks
        ) * intensity;
    }

#ifdef ENABLE_SHADOWS
    vec4 world_position = vec4(v_frag_position, 1.0);
    vec4 light_camera_clip_space_position = light_camera.projection * light_camera.view * world_position;
    vec3 light_camera_ndc = light_camera_clip_space_position.xyz / light_camera_clip_space_position.w;
    vec3 projected_position = light_camera_ndc * 0.5 + 0.5;

    vec3 light_camera_direction = light_camera.camera_direction; // 正确获取光源方向
    float bias = max(0.0005, shadow_bias * (1.0 - dot(normalized_normal, -light_camera_direction)));

    float shadow = pcf(
        projected_position,
        pcf_radius,
        pcf_tightness,
        pcf_sample_count,
        bias,
        dl_shadow_map
    );

    // 调试阴影
    // float shadow_depth = texture(dl_shadow_map, projected_position.xy).r;
    // frag_color = vec4(vec3(shadow_depth), 1.0);
    
    frag_color = vec4(ambient + (1.0 - shadow) * (diffuse + specular), alpha);

#else
    frag_color = vec4(ambient + diffuse + specular, alpha);

#endif

}