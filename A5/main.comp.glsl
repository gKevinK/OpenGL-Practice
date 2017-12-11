#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D img;

#define EPSILON 0.00001

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

Sphere sph = Sphere(vec3(0.0), vec3(1.0), 1.0, 1.0, 1.0);

void main()
{
    vec4 pixel = vec4(0.1, 0.1, 0.1, 1.0);
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 up = vec2(p) / vec2(width, height);
    //ivec2 lp = ivec2(gl_LocalInvocationID.xy);
    StackFrame frames[16];
    
    Ray ray;
    ray.origin = viewPos;
    ray.dir = normalize(mix(mix(r00, r01, up.x), mix(r10, r11, up.x), up.y));


    if (hitSphere(sph, ray))
        pixel = vec4(1.0, 0.0, 0.0, 1.0);

    imageStore(img, p, pixel);
}

bool hitSphere(Sphere s, Ray r)
{
    vec3 L = s.center - r.origin;
    float tca = dot(L, r.dir);
    float d2 = dot(L, L) - tca * tca;
    if (d2 > s.radius * s.radius)
        return false;
    float thc = sqrt(s.radius * s.radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
//    if (t0 < t1 && t1 >= 0.0)
//        return vec2(t0, t1);
    return true;
}

bool hitTriangle(Triangle triang, Ray ray)
{
    // TODO
    return false;
}

