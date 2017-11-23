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

struct Ray
{
    glm::vec3 Origin;
    glm::vec3 Dir;
};

#endif // !DATA_H

