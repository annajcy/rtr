#define PI 3.141592653589793
#define PI2 6.283185307179586
#define PI_HALF 1.5707963267948966
#define PI_QUARTER 0.7853981633974483

#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_SPOT_LIGHT 8
#define MAX_POINT_LIGHT 8

#define MAX_SAMPLE_COUNT 128 // Keep this for PCF-style blurring of moments

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

// ... (parallax functions - unchanged) ...
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
layout(binding = 5) uniform sampler2D dl_shadow_map; // Assumed to be an RG texture

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

uniform float shadow_bias; // Standard bias
uniform float vsm_min_variance = 0.00002; // Minimum variance to prevent division by zero and reduce light bleeding
uniform float vsm_light_bleed_reduction = 0.15; // Controls how much to reduce light bleeding, adjust as needed

uniform float pcf_radius = 0.005; // Radius for blurring moments, in UV space. Adjust based on shadow map size.
uniform int pcf_sample_count = 16; // Number of samples for blurring moments
uniform float pcf_tightness = 0.8; // For Poisson disk sample distribution

// Poisson disk sampling (can be reused for blurring moments)
float rand_2to1(vec2 uv) {
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

void generate_poisson_disk_samples(
    vec2 uv_seed,
    int sample_count,
    float tightness,
    out vec2 samples[MAX_SAMPLE_COUNT]
) {
    float angle = rand_2to1(uv_seed) * PI2;
    float radius_step = 1.0 / float(min(sample_count, MAX_SAMPLE_COUNT)); // Ensure we don't exceed array
    float current_radius = radius_step;
    // Golden angle for spiral distribution, can be adjusted
    float angle_increment = (1.0 - 0.6180339887) * PI2; // Approx (sqrt(5)-1)/2 * 2PI

    for (int i = 0; i < min(sample_count, MAX_SAMPLE_COUNT); i++) {
        // Using pow on radius can distribute samples more towards the center or edge
        // For a more uniform disk, use sqrt(current_radius_normalized) * actual_radius
        // Here, we'll scale by pcf_radius later.
        float r = pow(current_radius, tightness);
        samples[i] = vec2(cos(angle) * r, sin(angle) * r);
        angle += angle_increment;
        current_radius += radius_step;
    }
}

float calculate_pcf_shadow_visibility(
    sampler2D shadow_map_sampler, // This is pcf_depth_shadow_map
    vec2 initial_uv,
    float receiver_depth,        // Current fragment's depth from light [0,1]
    float bias
) {
    if (initial_uv.x < 0.0 || initial_uv.x > 1.0 || initial_uv.y < 0.0 || initial_uv.y > 1.0 || receiver_depth >= 1.0) {
        return 1.0; // Not in shadow map range
    }

    float shadow = 0.0;
    int current_pcf_sample_count = min(pcf_sample_count, MAX_SAMPLE_COUNT);

    vec2 poisson_samples[MAX_SAMPLE_COUNT];
    generate_poisson_disk_samples(initial_uv, current_pcf_sample_count, pcf_tightness, poisson_samples);

    for (int i = 0; i < current_pcf_sample_count; i++) {
        vec2 sample_uv = initial_uv + poisson_samples[i] * pcf_radius;
        float occluder_depth = texture(shadow_map_sampler, clamp(sample_uv, 0.0, 1.0)).r; // Sample depth
        if (receiver_depth - bias > occluder_depth) { // Check if occluded
            shadow += 1.0;
        }
    }
    shadow /= float(current_pcf_sample_count); // Average occlusion

    return 1.0 - shadow; // Visibility = 1.0 - average_occlusion
}

// Calculates the shadow visibility using VSM with PCF-style moment blurring
float calculate_vsm_shadow_visibility(
    sampler2D shadow_map_sampler,
    vec2 initial_uv,              // UV in shadow map for the current fragment
    float receiver_depth,         // Current fragment's depth from light [0,1]
    float bias
) {
    if (initial_uv.x < 0.0 || initial_uv.x > 1.0 || initial_uv.y < 0.0 || initial_uv.y > 1.0 || receiver_depth >= 1.0) {
        return 1.0; // Outside shadow map or behind far plane - fully lit
    }

    vec2 blurred_moments = vec2(0.0, 0.0);
    int current_sample_count = min(pcf_sample_count, MAX_SAMPLE_COUNT);

    if (current_sample_count <= 1) { // No blurring or single sample
        blurred_moments = texture(shadow_map_sampler, initial_uv).rg;
    } else {
        vec2 poisson_samples[MAX_SAMPLE_COUNT];
        generate_poisson_disk_samples(initial_uv, current_sample_count, pcf_tightness, poisson_samples);

        for (int i = 0; i < current_sample_count; i++) {
            vec2 sample_uv = initial_uv + poisson_samples[i] * pcf_radius;
            blurred_moments += texture(shadow_map_sampler, clamp(sample_uv, 0.0, 1.0)).rg;
        }
        blurred_moments /= float(current_sample_count);
    }

    //blurred_moments = texture(shadow_map_sampler, initial_uv).rg;

    // E[depth], E[depth^2]
    float E_depth = blurred_moments.x;
    float E_depth_sq = blurred_moments.y;

    // Apply bias to the receiver depth (can also be applied to E_depth)
    float biased_receiver_depth = receiver_depth - bias;

    // If the fragment is closer to the light than the average occluder depth, it's lit.
    if (biased_receiver_depth <= E_depth) {
        return 1.0; // Fully lit
    }

    // Calculate variance
    float variance = E_depth_sq - (E_depth * E_depth);
    variance = max(variance, vsm_min_variance); // Clamp to minimum variance

    // Chebyshev's inequality
    float d = biased_receiver_depth - E_depth; // Difference between receiver and mean
    float p_max = variance / (variance + d * d); // Probability of being lit

    // Reduce light bleeding
    // This is one common heuristic. Others involve powers or smoothstep.
    // We want to reduce p_max when d is small but positive.
    p_max = smoothstep(vsm_light_bleed_reduction, 1.0, p_max);
    
    return p_max; // p_max is visibility (1.0 = lit, 0.0 = occluded)
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

    // Calculate lighting (Directional, Point, Spot)
    // ... (Your existing lighting loops for dl_count, pl_count, spl_count remain the same) ...
    // Example for directional light (repeat for point and spot)
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

        // Spotlights also have distance attenuation, not shown in your original struct, but usually present
        // float distance = length(light_vector);
        // float attenuation = ... ;
        // spot_effect *= attenuation;


        diffuse += calculate_diffuse(
            normalized_normal,
            light_dir,
            spl_lights[i].intensity,
            spl_lights[i].color,
            albedo.rgb,
            kd
        ) * spot_effect; // Use spot_effect which includes the angular cutoff

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
        ) * spot_effect; // Use spot_effect
    }


#ifdef ENABLE_SHADOWS
    float shadow_visibility = 1.0; // Default to fully lit

    // Assuming directional light shadow for now, adapt if you have shadows for other light types
    if (dl_count > 0) { // Or apply shadows selectively per light
        vec4 world_position = vec4(v_frag_position, 1.0);
        vec4 light_camera_clip_space_pos = light_camera.projection * light_camera.view * world_position;
        vec3 light_camera_ndc = light_camera_clip_space_pos.xyz / light_camera_clip_space_pos.w;
        vec2 shadow_map_uv = light_camera_ndc.xy * 0.5 + 0.5; // Transform from [-1,1] to [0,1]
        float receiver_depth_from_light = light_camera_ndc.z * 0.5 + 0.5; // Depth from light's perspective [0,1]

        // Calculate normal-based bias
        // Light direction for bias calculation should be the actual light source direction
        // For directional lights, this is -dl_lights[0].direction (if using the first DL for shadow)
        // or light_camera.camera_direction if the light camera is perfectly aligned.
        // Let's assume light_camera.camera_direction represents the main shadow casting light's direction.
        vec3 light_dir_for_bias = normalize(light_camera.camera_direction); // This is direction TO the light from origin
                                                                     // We need direction FROM the light, or use dot with -normal
        
        // A common bias formulation:
        float NdotL = dot(normalized_normal, -light_dir_for_bias); // light_dir_for_bias should be direction *from* the light source
                                                              // If light_camera.camera_direction is view direction of light camera, then it's effectively -light_direction
                                                              // So, dot(normalized_normal, light_camera.camera_direction) might be what you need. Test this.
                                                              // Or, more simply, use the directional light's direction:
        // NdotL = dot(normalized_normal, normalize(-dl_lights[0].direction)); // if dl_shadow_map corresponds to dl_lights[0]
        
        // Using a potentially more robust bias, adjust factors as needed
        float bias = max(shadow_bias * (1.0 - NdotL), 0.0005); // shadow_bias is a uniform like 0.005

        shadow_visibility = calculate_vsm_shadow_visibility(
            dl_shadow_map,
            shadow_map_uv,
            receiver_depth_from_light,
            bias
        );
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
