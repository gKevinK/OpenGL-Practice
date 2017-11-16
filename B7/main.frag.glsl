#version 430 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, TexCoord);
    FragColor = vec4(Normal, 1.0);
}