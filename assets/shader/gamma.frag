// gamma.frag

out vec4 frag_color;
in vec2 v_uv;

layout(binding = 0) uniform sampler2D u_screen_texture;

void main() {
    vec4 color = texture(u_screen_texture, v_uv);
    frag_color = vec4(pow(color.rgb, vec3(1.0 / 2.2)), 1.0);
}