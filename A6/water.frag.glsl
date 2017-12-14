#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Wave {
    vec3 dir;
    float fraq;
    float length;
    float amp;
};

uniform float time;
uniform vec3 viewPos;
uniform vec3 dirLight;
uniform mat4 reflViewMat;
uniform mat4 reflProjMat;
uniform sampler2D reflColor;
uniform sampler2D reflDepth;
uniform sampler2D waterTex;
uniform Wave waves[8];
uniform int waveNum;

float fresnel(float cosine);
vec3 reflTexCoord(vec3 pos);
//vec3 reflDirTexCoord(vec3 dir);
vec3 hit(vec3 pos, vec3 dir);

const float R0 = (0.33 * 0.33) / (2.33 * 2.33);

void main()
{
    // Wave
	vec3 norm = normalize(Normal);
    //for (int i = 0; i < waveNum; i++) {
    //    norm += waves[i].amp * normalize(waves[i].dir * sin(0));
    //}
    norm += 0.005 * normalize(vec3(1.0, 0.0, 3.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(20  * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(3.0, 0.0, 1.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(20  * (0.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(1.0, 0.0, 3.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(5   * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(3.0, 0.0, 1.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(5   * (0.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(1.0, 0.0, 3.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(1.5 * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(3.0, 0.0, 1.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(1.5 * (0.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm.xz = norm.xz * max(0.0, 1 - length(FragPos - viewPos) / viewPos.y / 10);
    norm = normalize(norm);

	vec3 reflectDir = reflect(normalize(FragPos - viewPos), norm);
    if (reflectDir.y < 0) reflectDir.y = 0.0;
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
    vec4 texColor = texture(waterTex, FragPos.xz + 20 * norm.xz); //  + vec2(-0.1, 0.1) * time

    // Mix reflection and refraction
    float R = fresnel(dot(norm, reflectDir));
    FragColor = reflectColor * R + texColor * (1 - R) * 0.5;
    if (length(reflectDir - dirLight) < 0.2)
        FragColor = mix(reflectColor, FragColor, length(reflectDir - dirLight) / 0.4 + 0.5);
}

float fresnel(float cosine)
{
    float a = 1 - cosine;
    float aa = a * a;
    return R0 + (1 - R0) * aa * aa * a;
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

vec3 hit(vec3 pos, vec3 dir)
{
    float dl = max(0, 0);
    vec3 l = pos + dir * dl;
	// 
	for (int i = 0; i < 10; i++) {
		// 
		l = pos + dir * dl;
	}
    return l;
}

/*
vec3 Hit(vec3 x, vec3 R) {
	float rl = texCUBE(mp, R).a; // |r|
	float dp = rl - dot(x, R);
	vec3 p = x + R * dp;
	float ppp = length(p)/texCUBE(mp,p).a;
	float dun =0, dov =0, pun = ppp, pov = ppp;
	if (ppp < 1) dun = dp; else dov = dp;
	float dl = max(dp + rl * (1 - ppp), 0);
	vec3 l = x + R * dl;
	// iteration
	for(int i = 0; i < NITER; i++) {
		float ddl;
		float llp = length(l)/texCUBE(mp,l).a;
		if (llp < 1) { // undershooting
			dun = dl; pun = llp;
			ddl = (dov == 0) ? rl * (1 - llp) :
				(dl-dov) * (1-llp)/(llp-pov);
		} else { // overshooting
			dov = dl; pov = llp;
			ddl = (dun == 0) ? rl * (1 - llp) :
				(dl-dun) * (1-llp)/(llp-pun);
		}
		dl = max(dl + ddl, 0); // avoid flip
		l = x + R * dl;
	}
	return l;
}
*/