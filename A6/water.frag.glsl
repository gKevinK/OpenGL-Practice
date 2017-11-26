#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform float time;
uniform vec3 viewPos;
uniform mat4 reflViewMat;
uniform mat4 reflProjMat;
uniform sampler2D reflColor;
uniform sampler2D reflDepth;
uniform sampler2D waterTex;

vec3 reflTexCoord(vec3 pos);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 reflectDir = reflect(normalize(FragPos - viewPos), norm);
	vec3 fragCoord = reflTexCoord(FragPos);
	vec3 endCoord = reflTexCoord(reflectDir);
	float rLength = 1.0 / length(fragCoord.xy - endCoord.xy);
	for (float i = 0; i < 1;) {
		vec3 coord = mix(fragCoord, endCoord, i);
		float depth = texture(reflDepth, coord.xy).r;
		if (depth < reflTexCoord(coord).z) {
			FragColor = texture(reflColor, coord.xy);
			return;
		}
		//i += max(0.05, rLength / 20);
		i += 0.02;
	}

	FragColor = texture(reflColor, fragCoord.xy);
}

vec3 reflTexCoord(vec3 pos)
{
	vec4 texCoord_t = reflProjMat * reflViewMat * vec4(pos, 1.0);
	return vec3(0.5, 0.5, 0.0) + vec3(0.5, 0.5, 1.0) * texCoord_t.xyz / texCoord_t.w;
}
