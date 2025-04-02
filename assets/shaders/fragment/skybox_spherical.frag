#version 460 core
out vec4 FragColor;

in vec3 UVW;

uniform sampler2D mainTex;

const float PI = 3.1415926535897932384626433832795;

vec2 UVWToUV(vec3 uvw) {
	vec3 uvwN = normalize(uvw);
	float u = atan(uvwN.z, uvwN.x) / (2.0 * PI) + 0.5;
	float v = asin(uvwN.y) / PI + 0.5;
	return vec2(u, v);
}

void main()
{
	vec2 uv = UVWToUV(UVW);
	FragColor = texture(mainTex, uv);
}