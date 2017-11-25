#version 430 core

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
}