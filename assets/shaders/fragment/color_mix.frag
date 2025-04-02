#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture0;  // 第一个颜色缓冲
uniform sampler2D texture1;  // 第二个颜色缓冲

void main()
{
    vec4 color0 = texture(texture0, TexCoords);
    vec4 color1 = texture(texture1, TexCoords);
    FragColor = mix(color0, color1, 0.5); // 混合两个缓冲区
}