#version 460 core
out vec4 frag_color;

in vec2 uv;
uniform sampler2D main_tex;

void main()
{
	frag_color = texture(main_tex, uv);
}