// skybox_cubemap.frag

out vec4 frag_color;
in vec3 v_uvw;

layout(binding = 0) uniform samplerCube u_cubemap;

void main() {
    frag_color = texture(u_cubemap, v_uvw);
}