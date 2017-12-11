#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include "shader.h"
#include "shaders.glsl.hpp"

#include "window.hpp"

#define gv3 glm::vec3
#define gm4 glm::mat4

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

unsigned int ScrWidth = 512;
unsigned int ScrHeight = 512;
float lastFrame;
float deltaTime;


int main(int argc, char ** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(ScrWidth, ScrHeight, "A-5", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

#ifdef _DEBUG
    unsigned int mainShader = loadShaderProgram("main.vert.glsl", "main.frag.glsl");
    unsigned int compShader = loadShaderProgram("main.comp.glsl");
#else
    unsigned int mainShader = loadShaderProgramS(main_vert_glsl, main_frag_glsl);
    unsigned int compShader = loadShaderProgramS(main_comp_glsl);
#endif // _DEBUG

    Window win;
    win.Init();

    unsigned int frame;
    glGenTextures(1, &frame);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScrWidth, ScrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindImageTexture(0, frame, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int frameRate = 0;
    int lastSecond = 0;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        frameRate++;
        if ((int)currentFrame > lastSecond) {
            std::cout << "\rFrame Rate:  " << frameRate << "  ";
            lastSecond = (int)currentFrame;
            frameRate = 0;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unsigned shader;

        shader = compShader;
        glUseProgram(shader);
        setTexture2D(shader, "img", 0, frame);
        setInt(shader, "width", ScrWidth);
        setInt(shader, "height", ScrHeight);
        setVec3(shader, "viewPos", gv3(0.0f, 0.0f, 5.0f));
        setVec3(shader, "r00", glm::normalize(gv3(-1.0f, -1.0f, -2.0f)));
        setVec3(shader, "r01", glm::normalize(gv3( 1.0f, -1.0f, -2.0f)));
        setVec3(shader, "r10", glm::normalize(gv3(-1.0f,  1.0f, -2.0f)));
        setVec3(shader, "r11", glm::normalize(gv3( 1.0f,  1.0f, -2.0f)));
        setVec3(shader, "dirLight.direction", gv3(0.0f, -1.0, 0.0f));
        setVec3(shader, "dirLight.color", gv3(1.0f, 1.0f, 1.0f));
        glDispatchCompute(ScrWidth / 8, ScrHeight / 8, 1);
        
        shader = mainShader;
        glUseProgram(shader);
        setTexture2D(shader, "tex", 0, frame);
        win.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    ScrWidth = width;
    ScrHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
