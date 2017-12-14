#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <stb_image.h>
#include <iostream>
#include "shader.h"
#include "shaders.glsl.hpp"

#include "window.hpp"

#define gv3 glm::vec3
#define gm4 glm::mat4

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadCubeMap(const std::vector<std::string> & paths);

unsigned int ScrWidth = 512;
unsigned int ScrHeight = 512;
float lastFrame;
float deltaTime;

unsigned int frame;

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
    glfwSwapInterval(2);
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

    unsigned int sphereBuf;
    glGenTextures(1, &frame);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScrWidth, ScrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindImageTexture(0, frame, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    std::vector<std::string> cubePaths =
    {
        "resource\\right.jpg",
        "resource\\left.jpg",
        "resource\\top.jpg",
        "resource\\bottom.jpg",
        "resource\\back.jpg",
        "resource\\front.jpg"
    };
    unsigned int skyboxCube = loadCubeMap(cubePaths);

    float spheres[] = {
        //0.0f, 0.0f, 0.0f,
        //1.0f, 1.0f, 0.0f,
        //1.0f, 1.0f, 20.0f,
        //1.0f, 1.5f, 0.0f,

        1.5f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.5f, 1.0f, 20.0f,

       -1.5f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        0.5f, 1.0f, 20.0f,

        0.5f,-0.2f, 2.5f,
        0.01f, 0.01f, 0.01f,
        0.5f, 1.0f, 1.5f,
    };
    glGenBuffers(1, &sphereBuf);
    glBindBuffer(GL_TEXTURE_BUFFER, sphereBuf);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(spheres), spheres, GL_STATIC_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, sphereBuf);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    
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
        //setTexture2D(shader, "spheres", 1, sphereTex);
        
        setInt(shader, "sphereNum", sizeof(spheres) / 9);
        setInt(shader, "width", ScrWidth);
        setInt(shader, "height", ScrHeight);
        setVec3(shader, "ambient", gv3(0.02f));
        setVec3(shader, "viewPos", gv3(0.0f, 0.0f, 5.0f));
        setVec3(shader, "r00", glm::normalize(gv3(-1.0f, -1.0f, -2.0f)));
        setVec3(shader, "r01", glm::normalize(gv3( 1.0f, -1.0f, -2.0f)));
        setVec3(shader, "r10", glm::normalize(gv3(-1.0f,  1.0f, -2.0f)));
        setVec3(shader, "r11", glm::normalize(gv3( 1.0f,  1.0f, -2.0f)));
        setVec3(shader, "dirLight.direction", gv3(0.0f, 1.0, 0.0f));
        setVec3(shader, "dirLight.color", gv3(1.0f, 1.0f, 1.0f));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCube);
        setInt(shader, "box", 1);
        glDispatchCompute(ScrWidth / 8, ScrHeight / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScrWidth, ScrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

unsigned int loadCubeMap(const std::vector<std::string> & paths)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (int i = 0; i < paths.size(); i++)
    {
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << paths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}
