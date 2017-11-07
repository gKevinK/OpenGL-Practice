#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform bool Flat;

void main()
{
	FragColor = vec4(Color, 1.0);
}