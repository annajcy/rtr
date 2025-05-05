// shadow_caster.frag

void main() {
    // 直接将深度写入到深度缓冲区
    gl_FragDepth = gl_FragCoord.z;
}