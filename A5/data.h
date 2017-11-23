#ifndef DATA_H
#define DATA_H

#include <glm\glm.hpp>

struct Sphere
{
    glm::vec3 Position;
    glm::vec3 Color;
    float Radius;
    float Reflection;
    float Refraction;
    float RefractIndex;
};

struct Triangle
{
    glm::vec3 Position0;
};

#endif // !DATA_H

