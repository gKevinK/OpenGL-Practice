#version 430 core
in vec2 TexCoord;

layout(location = 0) out vec3 Color;

uniform sampler2D tex;

void main()
{
    Color = texture(tex, TexCoord).xyz;
}