#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D img;

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
uniform DirLight dirLight;

bool hitSphere(Sphere sphere, Ray ray, out float d, out vec3 norm);
float sphereReflect(Sphere sphere, Ray ray, inout Ray rays, out int num);
//float sphereRefract(Sphere sphere, Ray ray, out Ray refr);
bool hitTriangle(Triangle triang, Ray ray, float d);

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float blinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess);

Sphere sph = Sphere(vec3(0.0), vec3(1.0), 1.0, 1.0, 1.0);

void main()
{
    vec4 pixel = vec4(0.1, 0.1, 0.1, 1.0);
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    vec2 up = vec2(p) / vec2(width, height);
    //ivec2 lp = ivec2(gl_LocalInvocationID.xy);
    StackFrame frames[16];
    
    Ray ray = Ray(viewPos, normalize(mix(mix(r00, r01, up.x), mix(r10, r11, up.x), up.y)), vec3(1.0));

    float dist = EPSILON;
    vec3 norm;
    Ray rs[2];
    if (hitSphere(sph, ray, dist, norm))
        pixel.xyz = calcDirLight(dirLight, norm, viewPos - ray.origin + dist * ray.dir);

    pixel = pow(pixel, vec4(1 / GAMMA));
    imageStore(img, p, pixel);
}

bool hitSphere(Sphere s, Ray r, out float d, out vec3 norm)
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
    norm = normalize(-(r.origin + d * r.dir - s.center));
    return true;
}

//float sphereReflect(Sphere s, Ray r, out Ray[] rs)
//{
//
//}

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
