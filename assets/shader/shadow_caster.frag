// shadow_caster.frag

layout(location = 0) out vec2 frag_out_moments;

void main() {
    // gl_FragCoord.z is the depth value in [0, 1] range
    float depth = gl_FragCoord.z;
    frag_out_moments = vec2(depth, depth * depth);
}