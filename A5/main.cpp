#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include "shader.h"
#include "shaders.glsl.hpp"

int main(int argc, char ** argv)
{

#ifdef _DEBUG
    unsigned int mainShader = loadShaderProgram("main.vert.glsl", "main.frag.glsl");
    unsigned int compShader = loadShaderProgram("main.comp.glsl");
#else
    unsigned int mainShader = loadShaderProgramS(main_vert_glsl, main_frag_glsl);
    unsigned int compShader = loadShaderProgramS(main_comp_glsl);
#endif // _DEBUG



    return 0;
}