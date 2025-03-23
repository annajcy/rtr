#version 460 core
out vec4 frag_color;

in vec3 world_position;
in vec2 uv;
in vec3 normal;

uniform float time;
uniform sampler2D main_sampler;

uniform vec3 camera_position;

// x:ka, y:kd, z:ks, w: shiness
uniform vec4 material;

struct Ambient_light {
	vec3 color;
	float intensity;
};

struct Directional_light {
	vec3 color;
	float intensity;

	vec3 direction;
};

struct Point_light {
	vec3 color;
	float intensity;

	vec3 position;

	float k2;
	float k1;
	float kc;
};

struct Spot_light {
	vec3 color;
	float intensity;

	vec3 position;
	vec3 direction;

	float outer_angle;
	float inner_angle;
};

#define MAX_LIGHT_COUNT 10

uniform Ambient_light ambient_lights[MAX_LIGHT_COUNT];
uniform Point_light point_lights[MAX_LIGHT_COUNT];
uniform Directional_light directional_lights[MAX_LIGHT_COUNT];
uniform Spot_light spot_lights[MAX_LIGHT_COUNT];

// x: al, y:dl z:pl w: spl
uniform ivec4 light_count;

vec3 diffuse(vec3 light_color, vec3 object_color, vec3 normal, vec3 light_direction, float intensity) {
	return intensity * light_color * object_color * max(dot(-light_direction, normal), 0.0);
}

vec3 specular(vec3 light_color, vec3 normal, vec3 light_direction, vec3 view_direction, float shiness, float intensity) {
	vec3 half_direction = normalize(-(light_direction + view_direction));
	if (dot(normal, -light_direction) <= 0.0) return vec3(0.0, 0.0, 0.0);
	return intensity * light_color * pow(max(dot(half_direction, normal), 0.0), shiness);
}

void main() {
	vec3 result = vec3(0.0, 0.0, 0.0);

	vec3 object_color = texture(main_sampler, uv).rgb;
	
	vec3 normal_n = normalize(normal);
	vec3 view_direction_n = normalize(world_position - camera_position);

	float ka = material.x;
	float kd = material.y;
	float ks = material.z;
	float shiness = material.w;
	
	//ambient
	for (int i = 0; i < light_count.x; i ++) {
		Ambient_light light = ambient_lights[i];
		result += ka * light.color * light.intensity * object_color;
	}

	//directional
	for (int i = 0; i < light_count.y; i ++) {
		Directional_light light = directional_lights[i];
		vec3 light_direction_n = normalize(light.direction);
		result += kd * diffuse(light.color, object_color, normal_n, light_direction_n, light.intensity);
		result += ks * specular(light.color, normal_n, light_direction_n, view_direction_n, shiness, light.intensity);
	}

	//point
	for (int i = 0; i < light_count.z; i ++) {
		Point_light light = point_lights[i];
		vec3 light_direction_n = normalize(world_position - light.position);
		float dist = length(world_position - light.position);
		float attenuation = 1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);
		result += kd * diffuse(light.color, object_color, normal_n, light_direction_n, light.intensity * attenuation);
		result += ks * specular(light.color, normal_n, light_direction_n, view_direction_n, shiness, light.intensity * attenuation);
	}

	//spot light
	for (int i = 0; i < light_count.w; i ++) {
		Spot_light light = spot_lights[i];
		vec3 light_direction_n = normalize(world_position - light.position);
		vec3 target_direction_n = normalize(light.direction);

		float gamma = dot(light_direction_n, target_direction_n);
		float attenuation = clamp((gamma - light.outer_angle) / (light.inner_angle - light.outer_angle), 0.0, 1.0);

		result += kd * diffuse(light.color, object_color , normal_n, light_direction_n, light.intensity * attenuation);
		result += ks * specular(light.color, normal_n, light_direction_n, view_direction_n, shiness, light.intensity * attenuation);
	}

	frag_color = vec4(result, 1.0);

}