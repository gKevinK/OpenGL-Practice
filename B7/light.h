#ifndef LIGHT_H
#define LIGHT_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <string>
#include "shader.h"

class PointLight
{
public:
    glm::vec3 Position;
    glm::vec3 Color;
    float Constant = 0.1f;
    float Linear = 1.0f;
    float Quadratic = 0.1f;

    PointLight(glm::vec3 position, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f))
        : Position(position), Color(color) {}

    void SetUniform(unsigned int program, const std::string & name) {
        setVec3(program, (name + ".position").c_str(), Position);
        setVec3(program, (name + ".color").c_str(), Color);
        setFloat(program, (name + ".constant").c_str(), Constant);
        setFloat(program, (name + ".linear").c_str(), Linear);
        setFloat(program, (name + ".quadratic").c_str(), Quadratic);
    }
};

class DirLight
{
public:
    glm::vec3 Direction;
    glm::vec3 Color;

    DirLight(glm::vec3 direction, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f))
        : Direction(direction), Color(color) {}

    void SetUniform(unsigned int program, const std::string & name) {
        setVec3(program, (name + ".direction").c_str(), Direction);
        setVec3(program, (name + ".color").c_str(), Color);
    }
};

class SpotLight
{
public:
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Color;
    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
    float CutOff = glm::cos(glm::radians(12.5f));
    float OuterCutOff = glm::cos(glm::radians(15.0f));

    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f))
        : Position(position), Direction(direction), Color(color) {}

    void SetUniform(unsigned int program, const std::string & name) {
        setVec3(program, (name + ".position").c_str(), Position);
        setVec3(program, (name + ".direction").c_str(), Direction);
        setVec3(program, (name + ".color").c_str(), Color);
        setFloat(program, (name + ".constant").c_str(), Constant);
        setFloat(program, (name + ".linear").c_str(), Linear);
        setFloat(program, (name + ".quadratic").c_str(), Quadratic);
        setFloat(program, (name + ".cutOff").c_str(), CutOff);
        setFloat(program, (name + ".outerCutOff").c_str(), OuterCutOff);
    }
};

#endif // !LIGHT_H
#ifndef LIGHT_H
#define LIGHT_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <string>
#include "shader.h"

class PointLight
{
public:
    glm::vec3 Position;
    glm::vec3 Color;
    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;

    PointLight(glm::vec3 position, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f))
        : Position(position), Color(color) {}

    void SetUniform(unsigned int program, const std::string & name) {
        setVec3(program, (name + ".position").c_str(), Position);
        setVec3(program, (name + ".color").c_str(), Color);
        setFloat(program, (name + ".constant").c_str(), Constant);
        setFloat(program, (name + ".linear").c_str(), Linear);
        setFloat(program, (name + ".quadratic").c_str(), Quadratic);
    }
};

class DirLight
{
public:
    glm::vec3 Direction;
    glm::vec3 Color;

    DirLight(glm::vec3 direction, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f))
        : Direction(direction), Color(color) {}

    void SetUniform(unsigned int program, const std::string & name) {
        setVec3(program, (name + ".direction").c_str(), Direction);
        setVec3(program, (name + ".color").c_str(), Color);
    }
};

class SpotLight
{
public:
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Color;
    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
    float CutOff = glm::cos(glm::radians(12.5f));
    float OuterCutOff = glm::cos(glm::radians(15.0f));

    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f))
        : Position(position), Direction(direction), Color(color) {}

    void SetUniform(unsigned int program, const std::string & name) {
        setVec3(program, (name + ".position").c_str(), Position);
        setVec3(program, (name + ".direction").c_str(), Direction);
        setVec3(program, (name + ".color").c_str(), Color);
        setFloat(program, (name + ".constant").c_str(), Constant);
        setFloat(program, (name + ".linear").c_str(), Linear);
        setFloat(program, (name + ".quadratic").c_str(), Quadratic);
        setFloat(program, (name + ".cutOff").c_str(), CutOff);
        setFloat(program, (name + ".outerCutOff").c_str(), OuterCutOff);
    }
};

#endif // !LIGHT_H
