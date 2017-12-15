#ifndef SHADERS_GLSL_HPP
#define SHADERS_GLSL_HPP

#include <string>

const std::string skybox_frag_glsl = R"=====(#version 430 core
in vec3 TexCoord;
out vec4 Color;

uniform samplerCube skybox;

void main()
{
    Color = texture(skybox, TexCoord);
})=====";

const std::string skybox_vert_glsl = R"=====(#version 430 core
layout(location = 0) in vec3 aPos;
out vec3 TexCoord;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    gl_Position = proj * view * vec4(aPos, 1.0);
    TexCoord = aPos;
})=====";

const std::string skybox2_frag_glsl = R"=====(#version 430 core
in vec2 TexCoord;

layout(location = 0) out vec3 Color;

uniform sampler2D tex;

void main()
{
    Color = texture(tex, TexCoord).xyz;
})=====";

const std::string skybox2_vert_glsl = R"=====(#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 proj;
uniform mat4 view;
uniform bool invert;

void main()
{
    if (invert) {
        gl_Position = proj * view * vec4(aPos.x, -aPos.y, aPos.z, 1.0);
    } else {
        gl_Position = proj * view * vec4(aPos, 1.0);
    }
    TexCoord = aTexCoord;
})=====";

const std::string terrain_frag_glsl = R"=====(#version 430 core

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
    float diff = max(dot(norm, lightDir), 0.0) * 0.6 + 0.4;

    vec3 textur = pow(texture(textureMap, TexCoord).rgb, vec3(gamma));
    vec3 detail = texture(detailMap, TexCoord * 10).rgb * 0.5 - vec3(0.25);
    FragColor = vec4(pow(diff * (textur + detail), vec3(1 / gamma)), 1.0);
})=====";

const std::string terrain_tesc_glsl = R"=====(#version 430 core

layout(vertices = 4) out;

in vec2 TexCoord_tc[];

out vec2 TexCoord_te[];

uniform vec3 viewPos;
uniform mat4 model;

void main()
{
    if (gl_InvocationID == 0) {
        float l = 0, d = 1;
        for (int i = 0; i < 4; i++)
            l += length((model * gl_in[gl_InvocationID].gl_Position).xz - viewPos.xz);
        if (l < 0.3 * 4)
            d = 32;
        else if (l < 0.5 * 4)
            d = 20;
        else if (l < 1 * 4)
            d = 12;
        else if (l < 3 * 4)
            d = 6;
        else
            d = 3;
        gl_TessLevelOuter[0] = d;
        gl_TessLevelOuter[1] = d;
        gl_TessLevelOuter[2] = d;
        gl_TessLevelOuter[3] = d;
        gl_TessLevelInner[0] = d;
        gl_TessLevelInner[1] = d;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TexCoord_te[gl_InvocationID] = TexCoord_tc[gl_InvocationID];
})=====";

const std::string terrain_tese_glsl = R"=====(#version 430 core

layout(quads, equal_spacing, cw) in;

in vec2 TexCoord_te[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float base;
uniform float scale;
uniform sampler2D heightMap;
//uniform sampler2D textureMap;

void main()
{
    vec3 pos0 = gl_in[0].gl_Position.xyz;
    vec3 pos1 = gl_in[1].gl_Position.xyz;
    vec3 pos2 = gl_in[2].gl_Position.xyz;
    vec3 pos3 = gl_in[3].gl_Position.xyz;
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    TexCoord = mix(mix(TexCoord_te[0], TexCoord_te[1], u), mix(TexCoord_te[2], TexCoord_te[3], u), v);

    vec3 pos = mix(mix(pos0, pos1, u), mix(pos2, pos3, u), v);
    pos.y = base + texture(heightMap, TexCoord).x * scale;
    gl_Position = proj * view * model * vec4(pos, 1.0);
	FragPos = vec3(model * vec4(pos, 1.0));

    vec3 d1 = vec3(0.001, texture(heightMap, vec2(TexCoord.x + 0.0005, TexCoord.y)).x - texture(heightMap, vec2(TexCoord.x - 0.0005, TexCoord.y)).x, 0.0);
    vec3 d2 = vec3(0.0, texture(heightMap, vec2(TexCoord.x, TexCoord.y + 0.0005)).x - texture(heightMap, vec2(TexCoord.x, TexCoord.y - 0.0005)).x, -0.001);
    Normal = normalize(cross(d1, d2));
})=====";

const std::string terrain_vert_glsl = R"=====(#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord_tc;

void main()
{
    gl_Position = vec4(aPos.x, 0.0, -aPos.y, 1.0);
    TexCoord_tc = aTexCoord;
}
)=====";

const std::string texture_frag_glsl = R"=====(#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{ 
    FragColor = texture(tex, TexCoord);
})=====";

const std::string water_frag_glsl = R"=====(#version 430 core

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
    norm += 0.01 * normalize(vec3(1.0, 0.0, 3.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(20  * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.01 * normalize(vec3(3.0, 0.0, 1.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(20  * (0.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm += 0.01 * normalize(vec3(1.0, 0.0, 3.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(5   * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.01 * normalize(vec3(3.0, 0.0, 1.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(5   * (0.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(1.0, 0.0, 3.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(1.5 * (3.0 * FragPos.x + 1.0 * FragPos.z + time));
    norm += 0.005 * normalize(vec3(3.0, 0.0, 1.0) + vec3(FragPos.x, 0.0, FragPos.z)) * sin(1.5 * (0.0 * FragPos.x + 3.0 * FragPos.z + time));
    norm.xz = norm.xz * max(0.0, 1 - length(FragPos - viewPos) / viewPos.y / 20);
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
        FragColor += reflectColor * max(0.0, 0.5 - length(reflectDir - dirLight) / 0.4);
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
*/)=====";

const std::string water_tesc_glsl = R"=====(#version 430 core

layout(vertices = 4) out;

//in vec2 TexCoord_tc[];
//
//out vec2 TexCoord_te[];

uniform vec3 viewPos;
uniform mat4 model;

void main()
{
    if (gl_InvocationID == 0) {
        float l = 0, d = 1;
        for (int i = 0; i < 4; i++)
            l += length((model * gl_in[gl_InvocationID].gl_Position).xyz - viewPos);
        if (l < 3 * 4)
            d = 40;
        else if (l < 6 * 4)
            d = 24;
        else if (l < 10 * 4)
            d = 12;
        else if (l < 20 * 4)
            d = 6;
        else
            d = 3;
		d = 1;
		// Left down right up column row
        gl_TessLevelOuter[0] = d;
        gl_TessLevelOuter[1] = d;
        gl_TessLevelOuter[2] = d;
        gl_TessLevelOuter[3] = d;
        gl_TessLevelInner[0] = d;
        gl_TessLevelInner[1] = d;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    //TexCoord_te[gl_InvocationID] = TexCoord_tc[gl_InvocationID];
})=====";

const std::string water_tese_glsl = R"=====(#version 430 core

layout(quads, equal_spacing, ccw) in;

in vec2 TexCoord_te[];

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec3 pos0 = gl_in[0].gl_Position.xyz;
    vec3 pos1 = gl_in[1].gl_Position.xyz;
    vec3 pos2 = gl_in[2].gl_Position.xyz;
    vec3 pos3 = gl_in[3].gl_Position.xyz;
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    //TexCoord = mix(mix(TexCoord_te[0], TexCoord_te[1], u), mix(TexCoord_te[2], TexCoord_te[3], u), v);

    vec3 pos = mix(mix(pos0, pos1, u), mix(pos2, pos3, u), v);
    gl_Position = proj * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));

    Normal = vec3(0.0, 1.0, 0.0);
})=====";

const std::string water_vert_glsl = R"=====(#version 430 core
layout(location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos.x, 0.0, -aPos.y, 1.0);
}
)=====";

const std::string water2_vert_glsl = R"=====(#version 430 core
layout(location = 0) in vec2 aPos;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec4 pos = vec4(aPos.x, 0.0, -aPos.y, 1.0);
    gl_Position = pos;
}
)=====";

const std::string window_vert_glsl = R"=====(#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform float x;
uniform float y;
uniform float w;
uniform float h;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, -1.0, 1.0); 
    TexCoord = aTexCoord;
})=====";

#endif