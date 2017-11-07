#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
// uniform mat4 proj;

out vec3 FragPos;
out flat vec3 ColorF;
out vec3 Color;

void main()
{
	gl_Position = view * model * vec4(aPos, 1.0);
	Color = aColor;
	ColorF = aColor;
}
