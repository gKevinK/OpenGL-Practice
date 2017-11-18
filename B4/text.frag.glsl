#version 430 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    FragColor = vec4(textColor.xyz, texture(text, TexCoord).r);
}
