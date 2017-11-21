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

/*const mat4 B = mat4(-1.0, 3.0,-3.0, 1.0,
                     3.0,-6.0, 3.0, 0.0,
                    -3.0, 3.0, 0.0, 0.0,
                     1.0, 0.0, 0.0, 0.0);
const mat4 D = transpose(mat4(0.0, -3.0,  6.0, -3.0,
                              0.0,  9.0,-12.0,  3.0,
                              0.0, -9.0,  6.0,  0.0,
                              0.0,  3.0,  0.0,  0.0));*/

void main()
{
	// Copy control points' position
    float x_count = xCount;
    float y_count = yCount;
    vec3 poss[25];
	int k = 0;
    for (int i = 0; i < yCount; i++) {
        for (int j = 0; j < xCount; j++) {
            poss[i * 5 + j] = gl_in[k].gl_Position.xyz;
			k++;
        }
    }

	// Degree elevation
    if (x_count == 4) {
        for (int i = 0; i < y_count; i++) {
            poss[i * 5 + 4] = poss[i * 5 + 3];
            poss[i * 5 + 3] = poss[i * 5 + 2] * 3 / 4 + poss[i * 5 + 3] * 1 / 4;
            poss[i * 5 + 2] = poss[i * 5 + 1] * 2 / 4 + poss[i * 5 + 2] * 2 / 4;
            poss[i * 5 + 1] = poss[i * 5 + 0] * 1 / 4 + poss[i * 5 + 1] * 3 / 4;
        }
        x_count = 5;
    }
    if (y_count == 4) {
        for (int j = 0; j < x_count; j++) {
            poss[4 * 5 + j] = poss[3 * 5 + j];
            poss[3 * 5 + j] = poss[2 * 5 + j] * 1 / 4 + poss[3 * 5 + j] * 3 / 4;
            poss[2 * 5 + j] = poss[1 * 5 + j] * 2 / 4 + poss[2 * 5 + j] * 2 / 4;
            poss[1 * 5 + j] = poss[0 * 5 + j] * 3 / 4 + poss[1 * 5 + j] * 1 / 4;
        }
        y_count = 5;
    }

	// Calculate position
    float u = gl_TessCoord.x;
	float u1 = 1 - u;
    float v = gl_TessCoord.y;
	float v1 = 1 - v;
    float bu[5], bv[5], du[5], dv[5];
    bu[0] = 1 * u1 * u1 * u1 * u1;
    bu[1] = 4 * u * u1 * u1 * u1;
    bu[2] = 6 * u * u * u1 * u1;
    bu[3] = 4 * u * u * u * u1;
    bu[4] = 1 * u * u * u * u;
    bv[0] = 1 * v1 * v1 * v1 * v1;
    bv[1] = 4 * v * v1 * v1 * v1;
    bv[2] = 6 * v * v * v1 * v1;
    bv[3] = 4 * v * v * v * v1;
    bv[4] = 1 * v * v * v * v;
    vec3 pos;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            pos += bu[j] * poss[i * 5 + j] * bv[i];
        }
    }
    gl_Position = proj * view * model * vec4(pos, 1.0);
    TexCoord = vec2(u, v);

	// Calculate derivative and normal vector
	du[0] = -4 * u1 * u1 * u1;
    du[1] = 4 * u1 * u1 * u1 - 12 * u * u1 * u1;
    du[2] = 12 * u * u1 * u1 - 12 * u * u * u1;
    du[3] = 12 * u * u * u1 - 4 * u * u * u;
    du[4] = 4 * u * u * u;
	dv[0] = -4 * v1 * v1 * v1;
    dv[1] = 4 * v1 * v1 * v1 - 12 * v * v1 * v1;
    dv[2] = 12 * v * v1 * v1 - 12 * v * v * v1;
    dv[3] = 12 * v * v * v1 - 4 * v * v * v;
    dv[4] = 4 * v * v * v;
	vec3 dpdu, dpdv;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			dpdu += du[j] * poss[i * 5 + j] * bv[i];
			dpdv += bu[j] * poss[i * 5 + j] * dv[i];
		}
	}
    Normal = normalize(cross(dpdu, dpdv));
}