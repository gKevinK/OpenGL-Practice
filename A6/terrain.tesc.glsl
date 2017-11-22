#version 430 core

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
        if (l < 0.5 * 4)
            d = 20;
        else if (l < 1 * 4)
            d = 12;
        else if (l < 4 * 4)
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
}