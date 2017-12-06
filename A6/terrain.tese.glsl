#version 430 core

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
}