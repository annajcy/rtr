#version 460 core
out vec4 frag_color;

in vec2 uv;

uniform sampler2D screen_texture_sampler;

uniform vec2 resolution;

uniform int is_invert;
uniform int is_grayscale;
uniform int is_blur;

void main()
{
	// if (is_invert == 1) {
	// 	frag_color = vec4(1.0 - texture(screen_texture_sampler, uv).rgb, 1.0);
	// 	return;
	// }

	// if (is_grayscale == 1) {
	// 	vec4 color = texture(screen_texture_sampler, uv);
	// 	float average = (color.r + color.g + color.b) / 3.0;
	// 	frag_color = vec4(average, average, average, 1.0);
	// 	return;
	// }

	// if (is_blur == 1) {

	// 	vec2 dx = vec2(1.0 / resolution.x, 0.0);
	// 	vec2 dy = vec2(0.0, 1.0 / resolution.y);

	// 	float kernel[9] = float[](
	// 		1.0, 2.0, 1.0,
	// 		2.0, 4.0, 2.0,
	// 		1.0, 2.0, 1.0
	// 	);

	// 	float sum = 0.0;
	// 	for (int i = 0; i < 9; i ++) {
	// 		sum += kernel[i];
	// 	}

	// 	vec3 sum_color = vec3(0.0);

	// 	for (int x = -1; x <= 1; x ++) {
	// 		for (int y = -1; y <= 1; y ++) {
	// 			vec2 uv_ = uv + x * dx + y * dy;
	// 			vec3 color = texture(screen_texture_sampler, uv_).rgb;
	// 			float weight = kernel[(x + 1) * 3 + (y + 1)];
	// 			sum_color += color * weight;
	// 		}
	// 	}

	// 	sum_color /= sum;
	// 	frag_color = vec4(sum_color, 1.0);
	// 	return;
	// }

	vec3 color = texture(screen_texture_sampler, uv).rgb;
	vec3 result = pow(color, vec3(1.0 / 2.2));

	frag_color = vec4(result, 1.0);
}