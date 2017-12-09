#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D img;

struct Sphere {
	vec3 center;
	vec3 color;
	float radius;
	float eta1;
	float eta2;
};

struct Triangle
{
    vec3 v1;
    vec3 v2;
    vec3 v3;
    vec3 color;
    float eta1;
    float eta2;
};

struct Ray {
	vec3 origin;
	vec3 dir;
};

struct StackFrame {
    Ray ray;
    vec3 weight;
};

uniform sampler2D spheres;
uniform sampler2D triangles;
uniform int sphereNum;
uniform int triangNum;

uniform int width;
uniform int height;
uniform vec3 viewPos;
uniform vec3 r00;
uniform vec3 r01;
uniform vec3 r10;
uniform vec3 r11;

bool hitSphere(Sphere sphere, Ray ray);
bool hitTriangle(Triangle triang, Ray ray);

int value(int a)
{
	if (a == 0) return 0;
	return 1 + value(a - 1);
}

StackFrame frames[16];

void main()
{
	vec4 pixel = vec4(0.1, 0.1, 0.1, 1.0);
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 up = vec2(p) / vec2(width, height);
    //ivec2 lp = ivec2(gl_LocalInvocationID.xy);

    Ray ray;
    ray.origin = viewPos;
    ray.dir = mix(mix(r00, r01, up.x), mix(r10, r11, up.x), up.y);

	int i = 1;
	while (true) {
		if (i == 0) break;
		i--;
	}

	imageStore(img, p, pixel);
}

bool hitSphere(Sphere sphere, Ray ray)
{
	return false;
}

bool hitTriangle(Triangle triang, Ray ray)
{
	return false;
}

