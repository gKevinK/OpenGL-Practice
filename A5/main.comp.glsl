#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D img;

//uniform image2D img;
uniform sampler2D spheres;

void main() {
	vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0);
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    //ivec2 localCoord = ivec2(gl_LocalInvocationID.xy);

	int i = 1, j = 0;
	while (true) {
		if (j == 0) {
			i++;
			if (i == 10) {
				j = 1;
			}
		} else if (i > 0)
			i--;
		else
			break;
	}
    pixel.y += j;

	imageStore(img, coord, pixel);
}

