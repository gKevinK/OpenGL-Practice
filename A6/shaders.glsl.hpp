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

void main()
{
    gl_Position = proj * view * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
})=====";

const std::string terrain_frag_glsl = R"=====(#version 430 core

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
)=====";

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