#define PI 3.141592653589793
#define PI2 6.283185307179586
#define PI_HALF 1.5707963267948966
#define PI_QUARTER 0.7853981633974483
#define e 2.718281828459045

#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8

#define MAX_SAMPLE_COUNT 64

in vec2 v_uv;
in vec3 v_frag_position;
in vec3 v_normal;

out vec4 frag_color;

struct Camera {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec3 camera_direction;
    float padding;
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

// Shadow map sampler (now samples RG for depth and depth^2)
layout(binding = 5) uniform sampler2D dl_shadow_map; 

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

#define SHADOW_MAP_LOD_LEVELS 6

uniform float shadow_bias; 
uniform float vsm_min_variance = 0.00002; 
uniform float vsm_light_bleed_reduction = 0.15;
uniform float light_size;

float vsm(
    sampler2D shadow_map_sampler,
    vec2 uv,             
    float receiver_depth,
    float shadow_kernal_size
) {
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || receiver_depth >= 1.0) {
        return 1.0; // Outside shadow map or behind far plane - fully lit
    }

    vec2 blurred_moments = textureLod(shadow_map_sampler, uv, shadow_kernal_size * (SHADOW_MAP_LOD_LEVELS - 1)).rg;

    // E[depth], E[depth^2]
    float E_depth = blurred_moments.x;
    float E_depth_sq = blurred_moments.y;

    if (receiver_depth <= E_depth) {
        return 1.0; 
    }

    // Calculate variance
    float variance = E_depth_sq - (E_depth * E_depth);
    variance = max(variance, vsm_min_variance); // Clamp to minimum variance

    // Chebyshev's inequality
    float d = receiver_depth - E_depth; // Difference between receiver and mean
    float p_max = variance / (variance + d * d); // Probability of being lit

    // Reduce light bleeding
    // This is one common heuristic. Others involve powers or smoothstep.
    // We want to reduce p_max when d is small but positive.
    p_max = smoothstep(vsm_light_bleed_reduction, 1.0, p_max);
    
    return p_max; // p_max is visibility (1.0 = lit, 0.0 = occluded)
}

float get_search_radius(
    float receiver_depth,
    float light_size
) {
    return (receiver_depth - light_camera.near) / receiver_depth * 
            light_size / (light_camera.right - light_camera.left);
}

float get_blocker_depth(
    sampler2D shadow_map_sampler,
    vec2 uv,
    float receiver_depth,
    float shadow_kernal_size
) {

    float avg_depth = textureLod(shadow_map_sampler, uv, shadow_kernal_size * (SHADOW_MAP_LOD_LEVELS - 1)).r;

    float n1_div_n = vsm(
        shadow_map_sampler,
        uv,
        receiver_depth,
        shadow_kernal_size
    );

    float n2_div_n = 1.0 - n1_div_n; // Probability of being occluded
    float blocker_depth = (avg_depth - n1_div_n * receiver_depth) / (n2_div_n); 
    return blocker_depth;
}

float get_penumbra(
    float receiver_depth,
    float blocker_depth,
    float light_size
) {
    return (receiver_depth - blocker_depth) / receiver_depth * 
            light_size / (light_camera.right - light_camera.left);
}

float vssm(
    sampler2D shadow_map_sampler,
    vec2 uv,
    float receiver_depth,
    float light_size
) {
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || receiver_depth >= 1.0) {
        return 1.0; // Outside shadow map or behind far plane - fully lit
    }

    float search_radius = get_search_radius(
        receiver_depth,
        light_size
    );

    float blocker_depth = get_blocker_depth(
        shadow_map_sampler,
        uv,
        receiver_depth,
        clamp(search_radius, 0.0, 1.0)
    );

    if (blocker_depth < 0.0) {
        return 1.0; // No blocker found, fully lit
    }

    float penumbra = get_penumbra(
        receiver_depth,
        blocker_depth,
        light_size
    );

    // float scale = 1000.0;
    // float offset = 0.0;
    penumbra = clamp(penumbra, 0.0, 1.0);

    return vsm(
        shadow_map_sampler,
        uv,
        receiver_depth,
        penumbra
    );
}

#endif // ENABLE_SHADOWS


void main() {
    vec3 camera_position = main_camera.camera_position;
    // vec3 camera_direction = main_camera.camera_direction; // Not directly used for view_direction calculation below

    vec3 view_direction = normalize(camera_position - v_frag_position);

#ifdef ENABLE_HEIGHT_MAP
    vec2 uv = parallax_occlusion_uv(
        v_uv,
        -view_direction, // Parallax needs view direction from surface to camera
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
    vec3 normal = normalize(v_tbn * normal_map_normal); // Ensure TBN is correct
    vec3 normalized_normal = normalize(normal); // Redundant if normal is already normalized
#else
    vec3 normalized_normal = normalize(v_normal);
#endif

#ifdef ENABLE_ALBEDO_MAP
    vec4 albedo = texture(albedo_map, uv);
#else
    // Default albedo based on normal, useful for debugging
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

    // Point lights
    for (int i = 0; i < pl_count; i++) {
        vec3 light_vector = pl_lights[i].position - v_frag_position;
        float distance = length(light_vector);
        vec3 light_dir = normalize(light_vector);
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

    // Spot lights
    for (int i = 0; i < spl_count; i++) {
        vec3 light_vector = spl_lights[i].position - v_frag_position;
        vec3 light_dir = normalize(light_vector);
        float spot_effect = calculate_spot_intensity(
            light_dir, // direction from fragment to light
            spl_lights[i].direction, // direction of spotlight
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
        ) * spot_effect; 

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
        ) * spot_effect; 
    }


#ifdef ENABLE_SHADOWS
    float shadow_visibility = 1.0; // Default to fully lit

    if (dl_count > 0) { 
        vec4 world_position = vec4(v_frag_position, 1.0);
        vec4 light_camera_clip_space_pos = light_camera.projection * light_camera.view * world_position;
        vec3 light_camera_ndc = light_camera_clip_space_pos.xyz / light_camera_clip_space_pos.w;
        vec2 shadow_map_uv = light_camera_ndc.xy * 0.5 + 0.5; // Transform from [-1,1] to [0,1]
        float receiver_depth = light_camera_ndc.z * 0.5 + 0.5; // Depth from light's perspective [0,1]

        vec3 light_dir_for_bias = normalize(light_camera.camera_direction); // This is direction TO the light from origin
        // A common bias formulation:
        float NdotL = dot(normalized_normal, -light_dir_for_bias); 
        float bias = max(shadow_bias * (1.0 - NdotL), 0.0005); // shadow_bias is a uniform like 0.005

        shadow_visibility = vssm(
            dl_shadow_map,
            shadow_map_uv,
            receiver_depth - bias,
            light_size
        );

        // shadow_visibility = vsm(
        //     dl_shadow_map,
        //     shadow_map_uv,
        //     receiver_depth - bias,
        //     light_size
        // );
    }

    frag_color = vec4(ambient + shadow_visibility * (diffuse + specular), alpha);

#else // No shadows

    frag_color = vec4(ambient + diffuse + specular, alpha);

#endif // ENABLE_SHADOWS

    // Handle potential discard for fully transparent fragments if needed
    if (alpha < 0.01) { // Example threshold
        discard;
    }
}
