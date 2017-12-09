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

uniform sampler2D spheres;
uniform sampler2D triangles;

uniform int width;
uniform int height;
uniform vec3 viewPos;
uniform vec3 r00;
uniform vec3 r01;
uniform vec3 r10;
uniform vec3 r11;

bool hitSphere(Sphere sphere, Ray ray);
bool hitTriangle(Triangle triangle, Ray ray);

void main() {
	vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0);
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 localCoord = ivec2(gl_LocalInvocationID.xy);

	if (length(coord - ivec2(width, height) / 2) < 100)
		pixel = vec4(1.0, 1.0, 0.0, 1.0);
	if (length(coord - ivec2(width, height) / 2) < 25)
		pixel = vec4(1.0, 1.0, 1.0, 1.0);

	imageStore(img, coord, pixel);
}

bool hitSphere(Sphere sphere, Ray ray)
{
	return false;
}

bool hitTriangle(Triangle triangle, Ray ray)
{
	return false;
}

