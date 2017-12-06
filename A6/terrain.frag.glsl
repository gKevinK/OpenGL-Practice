#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform vec3 dirLight;
uniform sampler2D textureMap;
uniform sampler2D detailMap;

const float gamma = 2.2;

void main()
{
	if (FragPos.y < 0) discard;
    vec3 lightDir = normalize(dirLight);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0) * 0.5 + 0.5;

    vec3 detail = texture(detailMap, TexCoord * 10).rgb * 0.5 - vec3(0.25);
    FragColor = vec4(pow(diff * (pow(texture(textureMap, TexCoord).rgb, vec3(gamma)) + detail), vec3(1 / gamma)), 1.0);
}