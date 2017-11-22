#version 430 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform sampler2D textureMap;

void main()
{
    FragColor = vec4(texture(textureMap, TexCoord).rgb, 1.0);
}