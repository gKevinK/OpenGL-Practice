#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

void inline setMatrix4fv(unsigned int program, const char *name, const glm::mat4 & mat)
{
	unsigned int loc = glGetUniformLocation(program, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void inline setVec3(unsigned int program, const char *name, float v1, float v2, float v3)
{
	unsigned int loc = glGetUniformLocation(program, name);
	glUniform3f(loc, v1, v2, v3);
}

void inline setVec3(unsigned int program, const char *name, const glm::vec3 & vec)
{
	unsigned int loc = glGetUniformLocation(program, name);
	glUniform3fv(loc, 1, glm::value_ptr(vec));
}

void setFloat(unsigned int program, const char *name, float value)
{
	unsigned int loc = glGetUniformLocation(program, name);
	glUniform1f(loc, value);
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
	const char *pvCode = vertexCode.c_str();
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &pvCode, NULL);
	glCompileShader(vertexShader);
	checkCompileErrors(vertexShader, "VERTEX");
	const char *pfCode = fragmentCode.c_str();
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &pfCode, NULL);
	glCompileShader(fragmentShader);
	checkCompileErrors(fragmentShader, "FRAGMENT");
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkCompileErrors(shaderProgram, "PROGRAM");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

#endif // !SHADER_H

