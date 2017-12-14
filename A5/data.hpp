#ifndef DATA_H
#define DATA_H

#include <glm\glm.hpp>

struct Sphere
{
    glm::vec3 Center;
    glm::vec3 Color;
    float Radius;
    float Eta1;
    float Eta2;
    float Reflect;
    float Transparency;
    float _a;
};

struct Triangle
{
    glm::vec3 V1;
    glm::vec3 V2;
    glm::vec3 V3;
    glm::vec3 Color;
    float Eta1;
    float Eta2;
    float _a;
};

//struct Ray
//{
//    glm::vec3 Origin;
//    glm::vec3 Dir;
//};

#endif // !DATA_H
