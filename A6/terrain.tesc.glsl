#version 430 core

layout(vertices = 4) out;

in vec2 TexCoord_tc[];

out vec2 TexCoord_te[];

uniform vec3 viewPos;
uniform mat4 model;

int level(float dist, int n);

void main()
{
    if (gl_InvocationID == 0) {
        float d0 = length((model * gl_in[0].gl_Position).xz - viewPos.xz);
        float d1 = length((model * gl_in[1].gl_Position).xz - viewPos.xz);
        float d2 = length((model * gl_in[2].gl_Position).xz - viewPos.xz);
        float d3 = length((model * gl_in[3].gl_Position).xz - viewPos.xz);
        // Left down right up column row
        gl_TessLevelOuter[0] = level(d0 + d2, 2);
        gl_TessLevelOuter[1] = level(d0 + d1, 2);
        gl_TessLevelOuter[2] = level(d1 + d3, 2);
        gl_TessLevelOuter[3] = level(d2 + d3, 2);
        gl_TessLevelInner[0] = level(d0 + d1 + d2 + d3, 4);
        gl_TessLevelInner[1] = gl_TessLevelInner[0];
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TexCoord_te[gl_InvocationID] = TexCoord_tc[gl_InvocationID];
}

int level(float dist, int n)
{
    int d = 3;
    if (dist < 0.5 * n)
        d = 40;
    else if (dist < 1 * n)
        d = 24;
    else if (dist < 2 * n)
        d = 12;
    else if (dist < 4 * n)
        d = 6;
    else
        d = 3;
    return d;
}
