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
//vec3 reflDirTexCoord(vec3 dir);

const float R0 = (0.33 * 0.33) / (2.33 * 2.33);

void main()
{
    // Wave
	vec3 norm = normalize(Normal);
    norm += 0.001 * normalize(vec3(1.0, 0.0, 3.0)) * sin(20 * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.001 * normalize(vec3(3.0, 0.0, 1.0)) * sin(20 * (1.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm = normalize(norm);

	vec3 reflectDir = reflect(normalize(FragPos - viewPos), norm);
	vec3 fragCoord = reflTexCoord(FragPos);
	//vec3 endCoord = reflDirTexCoord(reflectDir);

    // Ray marching
    vec2 hitCoord;
    float i;
	for (i = 0.0; i < 10.0; i += 0.2) {
		vec3 coord = reflTexCoord(FragPos + reflectDir * i);
		float depth = texture(reflDepth, coord.xy).r;
		if (depth < coord.z) {
            hitCoord = coord.xy;
            break;
		}
	}
    if (i >= 10.0)
    	hitCoord = reflTexCoord(FragPos + reflectDir * 100.0).xy;
    //if (gl_FragCoord.x < 400)
    //    FragColor = vec4(100 * norm.x, 0.0, 100 * norm.z, 1.0);
    vec4 reflectColor = texture(reflColor, hitCoord);
    vec4 texColor = texture(waterTex, FragPos.xz + 10 * norm.xz); //  + vec2(-0.1, 0.1) * time

    // Mix reflection and refraction
    float R = R0 + (1 - R0) * pow(1 - cos(dot(norm, -reflectDir)), 2);
    FragColor = mix(reflectColor, texColor, R);
}

vec3 reflTexCoord(vec3 pos)
{
	vec4 texCoord_t = reflProjMat * reflViewMat * vec4(pos, 1.0);
	return vec3(0.5, 0.5, 0.0) + vec3(0.5, 0.5, 1.0) * texCoord_t.xyz / texCoord_t.w;
}

//vec3 reflDirTexCoord(vec3 dir)
//{
//    vec4 texCoord_t = reflProjMat * reflViewMat * vec4(dir, 0.0);
//    return vec3(0.5, 0.5, 0.0) + vec3(0.5, 0.5, 1.0) * texCoord_t.xyz;
//}
