#version 430 core

layout(quads, equal_spacing, cw) in;

out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

const mat4 B = mat4(-1.0, 3.0,-3.0, 1.0,
                     3.0,-6.0, 3.0, 0.0,
                    -3.0, 3.0, 0.0, 0.0,
                     1.0, 0.0, 0.0, 0.0);
const mat4 D = mat4( 0.0,  0.0, 0.0, 0.0,
                    -3.0,  9.0,-9.0, 3.0,
                     6.0,-12.0, 6.0, 0.0,
                    -3.0,  3.0, 0.0, 0.0);

void main()
{
    mat4 matx, maty, matz;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matx[i][j] = gl_in[i * 4 + j].gl_Position.x;
            maty[i][j] = gl_in[i * 4 + j].gl_Position.y;
            matz[i][j] = gl_in[i * 4 + j].gl_Position.z;
        }
    }
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 bu = vec4(u*u*u, u*u, u, 1.0);
    vec4 bv = vec4(v*v*v, v*v, v, 1.0);
    float x = dot(B * bu * matx, B * bv);
    float y = dot(B * bu * maty, B * bv);
    float z = dot(B * bu * matz, B * bv);
    gl_Position = proj * view * model * vec4(x, y, z, 1.0);
	TexCoord = vec2(u, v);
    //float dxdu = dot(D * bu * matx, B * bv);
    //Normal = normalize(cross());
}