#version 430 core
out vec4 FragColor;

in flat vec3 ColorF;
in vec3 Color;

uniform bool Flat;

void main()
{
	if (Flat) {
		FragColor = vec4(ColorF, 1.0);
	} else {
		FragColor = vec4(Color, 1.0);
	}
}
