#version 460 core
out vec4 frag_color;

in vec3 uvw;

uniform sampler2D main_tex;

const float PI = 3.1415926535897932384626433832795;

vec2 uvw_to_uv(vec3 uvw) {
	vec3 uvw_n = normalize(uvw);
	float u = atan(uvw_n.z, uvw_n.x) / (2.0 * PI) + 0.5;
	float v = asin(uvw_n.y) / PI + 0.5;
	return vec2(u, v);
}

void main()
{
	vec2 uv = uvw_to_uv(uvw);
	frag_color = texture(main_tex, uv);
}