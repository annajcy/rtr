#version 460 core
out vec4 frag_color;

in vec2 uv;
in vec3 normal;

uniform float near;
uniform float far;

void main()
{
	float z_ndc = gl_FragCoord.z * 2.0 - 1.0;
	float liner_depth = 2.0 * near / (far + near - z_ndc *(far - near));
	vec3 color = vec3(liner_depth);
	//vec3 color = vec3(gl_FragCoord.z);
	frag_color = vec4(color, 1.0);
}