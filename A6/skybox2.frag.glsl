#version 430 core
in vec2 TexCoord;

out vec4 Color;

uniform sampler2D tex;

void main()
{
    Color = texture(tex, TexCoord);
}