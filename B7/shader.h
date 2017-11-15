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


void checkCompileErrors(GLuint shader, const std::string & type)
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

unsigned loadShader(GLenum SHADER, const char * path, const std::string & type)
{
    std::string code = loadFile(path);
    const char *pcode = code.c_str();
    unsigned int shader = glCreateShader(SHADER);
    glShaderSource(shader, 1, &pcode, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, type);
    return shader;
}

unsigned int loadShaderProgram(const char *vert_path, const char *frag_path, const char *geom_path = nullptr)
{
    unsigned int vert = loadShader(GL_VERTEX_SHADER, vert_path, "VERTEX");
    unsigned int geom = 0;
    if (geom_path != nullptr)
        geom = loadShader(GL_GEOMETRY_SHADER, geom_path, "GEOMETRY");
    unsigned int frag = loadShader(GL_FRAGMENT_SHADER, frag_path, "FRAGMENT");
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    if (geom_path != nullptr)
        glAttachShader(program, geom);
    glAttachShader(program, frag);
    glLinkProgram(program);
    checkCompileErrors(program, "PROGRAM");
    glDeleteShader(vert);
    if (geom_path != nullptr)
        glDeleteShader(geom);
    glDeleteShader(frag);
    return program;
}

unsigned int loadShaderProgram(const char *vert_path, const char *tes_c_path, const char * tes_e_path, const char *frag_path)
{
    unsigned int vert = loadShader(GL_VERTEX_SHADER, vert_path, "VERTEX");
    unsigned int tesc = loadShader(GL_TESS_CONTROL_SHADER, tes_c_path, "TESS CONTROL");
    unsigned int tese = loadShader(GL_TESS_EVALUATION_SHADER, tes_e_path, "TESS EVALUATION");
    unsigned int frag = loadShader(GL_FRAGMENT_SHADER, frag_path, "FRAGMENT");
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, tesc);
    glAttachShader(program, tese);
    glAttachShader(program, frag);
    glLinkProgram(program);
    checkCompileErrors(program, "PROGRAM");
    glDeleteShader(vert);
    glDeleteShader(tesc);
    glDeleteShader(tese);
    glDeleteShader(frag);
    return program;
}

#endif // !SHADER_H
