#version 430 core

layout(vertices = 4) out;

uniform float outerLevel, innerLevel;

void main()
{
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = outerLevel;
        gl_TessLevelOuter[1] = outerLevel;
        gl_TessLevelOuter[2] = outerLevel;
        gl_TessLevelOuter[3] = outerLevel;
        gl_TessLevelInner[0] = innerLevel;
        gl_TessLevelInner[1] = innerLevel;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    gl_out[gl_InvocationID].TexCoord = gl_in[gl_InvocationID].TexCoord;
}