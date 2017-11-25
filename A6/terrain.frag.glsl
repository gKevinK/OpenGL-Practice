#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform sampler2D textureMap;
uniform sampler2D detailMap;

void main()
{
	if (FragPos.y < 0) discard;
    vec3 detail = texture(detailMap, TexCoord * 10).rgb * 0.5 - vec3(0.25);
    FragColor = vec4(texture(textureMap, TexCoord).rgb + detail, 1.0);
}