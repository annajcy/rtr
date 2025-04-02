#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 UVW;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPosition;

void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);

	mat4 model = mat4(1.0);
	model[3] = vec4(cameraPosition, 1.0); // 直接设置第四列

	vec4 pos = projection * view * model * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
	UVW = aPos;
}