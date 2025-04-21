// skybox_spherical.frag

out vec4 frag_color;
in vec3 v_uvw;

layout(binding = 0) uniform sampler2D u_spherical_map;

const float PI = 3.1415926535897932384626433832795;

vec2 uvw2uv(vec3 uvw) {
	vec3 uvwN = normalize(uvw);
	float u = atan(uvwN.z, uvwN.x) / (2.0 * PI) + 0.5;
	float v = asin(uvwN.y) / PI + 0.5;
	return vec2(u, v);
}

void main() {
    vec2 uv = uvw2uv(v_uvw);
    frag_color = texture(u_spherical_map, uv);
}