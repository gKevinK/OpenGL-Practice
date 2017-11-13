#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void inline setMat2(unsigned int program, const std::string & name, const glm::mat2 & mat)
{
	glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void inline setMat3(unsigned int program, const std::string & name, const glm::mat3 & mat)
{
	glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void inline setMat4(unsigned int program, const std::string & name, const glm::mat4 & mat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void inline setVec3(unsigned int program, const std::string & name, float v1, float v2, float v3)
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), v1, v2, v3);
}

void inline setVec3(unsigned int program, const std::string & name, const glm::vec3 & vec)
{
	glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &vec[0]);
}

void inline setInt(unsigned int program, const std::string & name, int value)
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void inline setFloat(unsigned int program, const std::string & name, float value)
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void inline setBool(unsigned int program, const std::string & name, bool value)
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR : " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR : " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

std::string loadFile(const char *path)
{
	std::ifstream fileStream;
	std::string code;
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		fileStream.open(path);
		std::stringstream stream;
		stream << fileStream.rdbuf();
		fileStream.close();
		code = stream.str();
	} catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		std::cout << path << std::endl;
	}
	return code;
}

unsigned int LoadShaderProgram(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr)
{
	std::string vertexCode = loadFile(vertexPath);
	std::string fragmentCode = loadFile(fragmentPath);
	const char *pvertCode = vertexCode.c_str();
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &pvertCode, NULL);
	glCompileShader(vertexShader);
	checkCompileErrors(vertexShader, "VERTEX");
	const char *pfragCode = fragmentCode.c_str();
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &pfragCode, NULL);
	glCompileShader(fragmentShader);
	checkCompileErrors(fragmentShader, "FRAGMENT");
	unsigned int geomShader;
	if (geometryPath != nullptr) {
		std::string geometryCode = loadFile(geometryPath);
		const char *pgeomCode = geometryCode.c_str();
		geomShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geomShader, 1, &pgeomCode, NULL);
		glCompileShader(geomShader);
		checkCompileErrors(geomShader, "GEOMETRY");
	}
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (geometryPath != nullptr) {
		glAttachShader(shaderProgram, geomShader);
	}
	glLinkProgram(shaderProgram);
	checkCompileErrors(shaderProgram, "PROGRAM");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryPath != nullptr) {
		glDeleteShader(geomShader);
	}
	return shaderProgram;
}

#endif // !SHADER_H
