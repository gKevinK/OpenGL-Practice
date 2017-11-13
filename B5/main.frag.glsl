#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D tex;

void main()
{
    FragColor = vec4(color, texture(tex, TexCoord).r);
}