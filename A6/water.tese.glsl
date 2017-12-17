#version 430 core

layout(quads, equal_spacing, ccw) in;

in vec2 TexCoord_te[];

out vec3 FragPos;
out vec3 Normal;

struct Wave {
    float o;
    float s;
    float k;
};

struct WaveDir {
    vec3 dir;
    float a;
};

//uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float time;
uniform Wave waves[10];
uniform int waveNum;
uniform WaveDir waveDirs[7];
uniform int waveDirNum;

void main()
{
    vec3 pos0 = gl_in[0].gl_Position.xyz;
    vec3 pos1 = gl_in[1].gl_Position.xyz;
    vec3 pos2 = gl_in[2].gl_Position.xyz;
    vec3 pos3 = gl_in[3].gl_Position.xyz;
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec3 pos = mix(mix(pos0, pos1, u), mix(pos2, pos3, u), v);
    //for (int i = 0; i < waveNum; i++) {
    //    for (int j = 0; j < waveDirNum; j++) {
    //        pos.y += 0.01 * sqrt(2 * waves[i].s * waveDirs[j].a) * sin(dot(pos * 100, waveDirs[j].dir) * waves[i].k - waves[i].o * time);
    //    }
    //}
    //pos.y /= (1 + length(pos) / 5.0);
    gl_Position = proj * view * vec4(pos, 1.0);
    FragPos = pos;

    Normal = vec3(0.0, 1.0, 0.0);
}