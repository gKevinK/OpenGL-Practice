#ifndef POINT_LIGHT

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <string>
#include "shader.h"

class PointLight
{
public:
	glm::vec3 Position;
	float Constant = 1.0f;
	float Linear = 0.09f;
	float Quadratic = 0.032f;
	glm::vec3 Ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 Diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	PointLight(glm::vec3 position)
		: Position(position) {}

	void SetUniform(unsigned int program, const std::string & name) {
		setVec3(program, (name + ".position").c_str(), Position);
		setFloat(program, (name + ".constant").c_str(), Constant);
		setFloat(program, (name + ".linear").c_str(), Linear);
		setFloat(program, (name + ".quadratic").c_str(), Quadratic);
		setVec3(program, (name + ". ambient").c_str(), Ambient);
		setVec3(program, (name + ".diffuse").c_str(), Diffuse);
		setVec3(program, (name + ".specular").c_str(), Specular);
	}

private:

};



#endif // !POINT_LIGHT
