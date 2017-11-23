#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

void main()
{
    FragColor = vec4(Normal, 1.0);
}