#ifndef SHADERS_GLSL_HPP
#define SHADERS_GLSL_HPP

#include <string>

const std::string main_comp_glsl = R"=====(#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D img;

#define FARCUT 100.0
#define EPSILON 0.00001
#define GAMMA 2.2

struct DirLight {
    vec3 direction;
    vec3 color;
};

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
    vec3 weight;
};

// Stack
struct Stack {
    int size;
    Ray rs[8];
};
/* Inout version
void stackInit(inout Stack s)
{
    s.size = 0;
}

bool stackEmpty(inout Stack s)
{
    return s.size == 0;
}

void stackPush(inout Stack s, Ray r)
{
    s.rs[s.size] = r;
    s.size++;
}

Ray stackTop(inout Stack s)
{
    if (s.size == 0)
        return Ray(vec3(0.0), vec3(1.0), vec3(0.0));
    Ray r = s.rs[s.size - 1];
    return r;
}

Ray stackPop(inout Stack s)
{
    if (s.size <= 0)
        return Ray(vec3(0.0), vec3(1.0), vec3(0.0));
    s.size--;
    return s.rs[s.size];
}
*/

// Global variable version
Stack stack;

void stackInit()
{
    stack.size = 0;
}

bool stackEmpty()
{
    return stack.size == 0;
}

bool stackFull()
{
    return stack.size >= 8;
}

void stackPush(Ray r)
{
    stack.rs[stack.size] = r;
    stack.size++;
}

Ray stackTop()
{
    if (stack.size == 0)
        return Ray(vec3(0.0), vec3(1.0), vec3(0.0));
    return stack.rs[stack.size - 1];
}

Ray stackPop()
{
    if (stack.size <= 0)
        return Ray(vec3(0.0), vec3(1.0), vec3(0.0));
    stack.size--;
    return stack.rs[stack.size];
}

uniform samplerBuffer spheres;
//uniform sampler2D triangles;
uniform int sphereNum;
//uniform int triangNum;

uniform int width;
uniform int height;
uniform vec3 viewPos;
uniform vec3 r00;
uniform vec3 r01;
uniform vec3 r10;
uniform vec3 r11;
uniform vec3 ambient;
uniform DirLight dirLight;

Sphere getSphere(int i);
bool hitSphere(Sphere sphere, Ray ray, out float d);
vec3 calcSphere(Sphere sphere, Ray ray, out Ray rays[2], out int num);
//float sphereRefract(Sphere sphere, Ray ray, out Ray refr);
bool hitTriangle(Triangle triang, Ray ray, float d);

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float blinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess);

// Sphere sph = Sphere(vec3(0.0), vec3(1.0), 1.0, 1.0, 1.0);

void main()
{
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 up = vec2(p) / vec2(width, height);
    //ivec2 lp = ivec2(gl_LocalInvocationID.xy);
    
    // Stack stack;
    //stackInit(stack);
    Ray oRay = Ray(viewPos, normalize(mix(mix(r00, r01, up.x), mix(r10, r11, up.x), up.y)), vec3(1.0));
    stackPush(oRay);
    
    while (!stackEmpty()) {
        Ray ray = stackPop();
        float dist = FARCUT + 0.1;
        Ray rs[2];
        int rnum;
        Sphere s;// = Sphere(vec3(0.0), vec3(0.0), EPSILON, 0.0, 0.0);
        //Triangle t;
        for (int i = 0; i < sphereNum; i++) {
            Sphere st = getSphere(i);
            float d;
            if (hitSphere(st, ray, d)) {
                if (EPSILON < d && d < dist) {
                    dist = d;
                    s = st;
                }
            }
        }
        /* for (int i = 0; i < triangNum; i++) {}*/

        if (dist < FARCUT) {
            vec3 norm = calcSphere(s, ray, rs, rnum);
            for (int j = 0; j < rnum; j++) {
                if (!stackFull() && length(rs[j].weight) > 0.1) {
                    stackPush(rs[j]);
                }
            }

            pixel.rgb += s.color * ray.weight * calcDirLight(dirLight, norm, -ray.dir);
            pixel.rgb += s.color * ambient;
        }
    }

    pixel = pow(pixel, vec4(1 / GAMMA));
    imageStore(img, p, pixel);
}

Sphere getSphere(int i)
{
    return Sphere(
        texelFetch(spheres, i * 3 + 0).xyz,
        texelFetch(spheres, i * 3 + 1).rgb,
        texelFetch(spheres, i * 3 + 2).x, texelFetch(spheres, i * 3 + 2).y, texelFetch(spheres, i * 3 + 2).z);
}

bool hitSphere(Sphere s, Ray r, out float d)
{
    vec3 L = s.center - r.origin;
    float tca = dot(L, r.dir);
    float d2 = dot(L, L) - tca * tca;
    if (d2 > s.radius * s.radius)
        return false;
    float thc = sqrt(s.radius * s.radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    d = t0 > 0.0 ? t0 : t1;
    //norm = normalize(-(r.origin + d * r.dir - s.center));
    return true;
}

vec3 calcSphere(Sphere s, Ray r, out Ray rs[2], out int num)
{
    vec3 L = s.center - r.origin;
    float tca = dot(L, r.dir);
    float d2 = dot(L, L) - tca * tca;
    float thc = sqrt(s.radius * s.radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    float d = t0 > 0.0 ? t0 : t1;
    vec3 p = r.origin + d * r.dir;
    vec3 norm = normalize(p - s.center);
    num = 1;
    rs[0] = Ray(p, reflect(r.dir, norm), r.weight * vec3(0.5));
    //rs[1] = Ray(p, reflect(r.dir, norm), r.weight * vec3(0.5));
    return norm;
}

bool hitTriangle(Triangle triang, Ray ray)
{
    // TODO
    return false;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    //float spec = blinnPhong(normal, lightDir, viewDir, material.shininess);
    //return light.color * (diff * material.diffuse + spec * material.specular);
    float spec = blinnPhong(normal, lightDir, viewDir, 32);
    return light.color * (diff * 0.8 + spec * 1.0);
}

//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    float diff = max(dot(normal, lightDir), 0.0);
//    float spec = BlinnPhong(normal, lightDir, viewDir, material.shininess);
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance +
//        light.quadratic * (distance * distance));
//    return light.color * (diff * material.diffuse + spec * material.specular) * attenuation;
//}

float blinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess)
{
    vec3 halfway = normalize(lightDir + viewDir);
    return pow(max(dot(halfway, normal), 0.0), shininess);
}
)=====";

const std::string main_frag_glsl = R"=====(#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, TexCoord);
})=====";

const std::string main_vert_glsl = R"=====(#version 430 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoord;

uniform float x;
uniform float y;
uniform float w;
uniform float h;

void main()
{
	float xpos = mix(x, x + w, aPos.x);
	float ypos = mix(y, y + h, aPos.y);
    gl_Position = vec4(xpos, ypos, -1.0, 1.0); 
    TexCoord = aPos;
})=====";

#endif