#version 430 core

layout(vertices = 4) out;

in vec2 TexCoord_tc[];

out vec2 TexCoord_te[];

void main()
{
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = 4;
        gl_TessLevelOuter[1] = 4;
        gl_TessLevelOuter[2] = 4;
        gl_TessLevelOuter[3] = 4;
        gl_TessLevelInner[0] = 4;
        gl_TessLevelInner[1] = 4;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TexCoord_te[gl_InvocationID] = TexCoord_tc[gl_InvocationID];
}