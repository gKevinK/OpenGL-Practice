#version 430 core

layout(quads, equal_spacing, cw) in;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform int xCount;
uniform int yCount;

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
    if (false) {
    //if (xCount == 4 && yCount == 4) {
        //mat4 matx, maty, matz;
        //for (int i = 0; i < 4; i++) {
        //    for (int j = 0; j < 4; j++) {
        //        matx[i][j] = gl_in[i * 4 + j].gl_Position.x;
        //        maty[i][j] = gl_in[i * 4 + j].gl_Position.y;
        //        matz[i][j] = gl_in[i * 4 + j].gl_Position.z;
        //    }
        //}
        //float u = gl_TessCoord.x;
        //float v = gl_TessCoord.y;

        //vec4 bu = vec4(u*u*u, u*u, u, 1.0);
        //vec4 bv = vec4(v*v*v, v*v, v, 1.0);
        //vec4 bbu = B * bu;
        //vec4 bbv = B * bv;
        //float x = dot(bbu * matx, bbv);
        //float y = dot(bbu * maty, bbv);
        //float z = dot(bbu * matz, bbv);
        //gl_Position = proj * view * model * vec4(x, y, z, 1.0);
        //FragPos = vec3(x, y, z);
        //TexCoord = vec2(u, v);

        //vec4 dbu = D * bu;
        //vec4 dbv = D * bv;
        //float dxdu = dot(dbu * matx, bbv);
        //float dydu = dot(dbu * maty, bbv);
        //float dzdu = dot(dbu * matz, bbv);
        //float dxdv = dot(bbu * matx, dbv);
        //float dydv = dot(bbu * maty, dbv);
        //float dzdv = dot(bbu * matz, dbv);
        //Normal = normalize(cross(vec3(dxdu, dydu, dzdu), vec3(dxdv, dydv, dzdv)));
    } else {
        float x_count = xCount;
        float y_count = yCount;
        vec3 poss[25];
        for (int i = 0; i < yCount; i++) {
            for (int j = 0; i < xCount; j++) {
                poss[i * 5 + j] = gl_in[i * 5 + j].gl_Position.xyz;
            }
        }
        if (x_count == 4) {
            for (int i = 0; i < y_count; i++) {
                poss[i * 5 + 4] = poss[i * 5 + 3];
                //poss[i * 5 + 3] = 3 / 4 * poss[i * 5 + 2] + 1 / 4 * poss[i * 5 + 3];
                //poss[i * 5 + 2] = 2 / 4 * poss[i * 5 + 1] + 2 / 4 * poss[i * 5 + 2];
                //poss[i * 5 + 1] = 1 / 4 * poss[i * 5 + 0] + 3 / 4 * poss[i * 5 + 1];
            }
            //x_count = 5;
        }
        //if (y_count == 4) {
        //    for (int j = 0; j < x_count; j++) {
        //        poss[4 * 5 + j] = poss[3 * 5 + j];
        //        poss[3 * 5 + j] = 3 / 4 * poss[2 * 5 + j] + 1 / 4 * poss[3 * 5 + j];
        //        poss[2 * 5 + j] = 2 / 4 * poss[1 * 5 + j] + 2 / 4 * poss[2 * 5 + j];
        //        poss[1 * 5 + j] = 1 / 4 * poss[0 * 5 + j] + 3 / 4 * poss[1 * 5 + j];
        //    }
        //    y_count = 5;
        //}
        
        float u = gl_TessCoord.x;
        float v = gl_TessCoord.y;
        float bu[5], bv[5], du[5], dv[5];
        bu[0] = (1 - u) * (1 - u) * (1 - u) * (1 - u);
        bu[1] = 4 * u * (1 - u) * (1 - u) * (1 - u);
        bu[2] = 6 * u * u * (1 - u) * (1 - u);
        bu[3] = 4 * u * u * u * (1 - u);
        bu[4] = 1 * u * u * u * u;
        bv[0] = (1 - v) * (1 - v) * (1 - v) * (1 - v);
        bv[1] = 4 * v * (1 - v) * (1 - v) * (1 - v);
        bv[2] = 6 * v * v * (1 - v) * (1 - v);
        bv[3] = 4 * v * v * v * (1 - v);
        bv[4] = 1 * v * v * v * v;
        vec3 pos;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                //pos = pos + bu[i] * poss[i * 5 + j] * bu[j];
            }
        }

        //gl_Position = proj * view * model * vec4(pos, 1.0);
        TexCoord = vec2(u, v);


        Normal = normalize(cross(vec3(1.0), vec3(0.5)));
    }
}