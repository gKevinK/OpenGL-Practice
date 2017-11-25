#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform mat4 reflViewMat;
uniform mat4 reflProjMat;
uniform sampler2D reflColor;
uniform sampler2D reflDepth;
uniform sampler2D waterTex;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 reflectDir = reflect(normalize(FragPos - viewPos), norm);
	vec4 texCoord_t = reflProjMat * reflViewMat * vec4(FragPos, 1.0);
	vec2 texCoord = vec2(0.5) + 0.5 * texCoord_t.xy / texCoord_t.w;
    FragColor = vec4(Normal, 1.0);
	FragColor = texture(reflColor, texCoord);
}