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
const mat4 D = transpose(mat4(0.0, -3.0,  6.0, -3.0,
                              0.0,  9.0,-12.0,  3.0,
                              0.0, -9.0,  6.0,  0.0,
                              0.0,  3.0,  0.0,  0.0));

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
    vec4 bbu = B * bu;
    vec4 bbv = B * bv;
    float x = dot(bbu * matx, bbv);
    float y = dot(bbu * maty, bbv);
    float z = dot(bbu * matz, bbv);
    gl_Position = proj * view * model * vec4(x, y, z, 1.0);

	TexCoord = vec2(u, v);
    
    vec4 dbu = D * bu;
    vec4 dbv = D * bv;
    float dxdu = dot(dbu * matx, bbv);
    float dydu = dot(dbu * maty, bbv);
    float dzdu = dot(dbu * matz, bbv);
    float dxdv = dot(bbu * matx, dbv);
    float dydv = dot(bbu * maty, dbv);
    float dzdv = dot(bbu * matz, dbv);
    Normal = normalize(cross(vec3(dxdu, dydu, dzdu), vec3(dxdv, dydv, dzdv)));
}