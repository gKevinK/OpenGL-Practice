#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <stb_image.h>
#include <iostream>
#include "shader.h"
#include "shaders.glsl.hpp"
#include "fps_camera.hpp"

#include "window.hpp"
#include "data.hpp"

#define gv3 glm::vec3
#define gm4 glm::mat4

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadCubeMap(const std::vector<std::string> & paths);

unsigned int ScrWidth = 512;
unsigned int ScrHeight = 512;
float lastFrame;
float deltaTime;
Camera camera = Camera(gv3(0.0f, 0.0f, 6.0f));
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

    std::vector<Sphere> spheres = {
        { gv3( 0.0f, 0.0f, 0.0f), gv3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f, 500.0f, 0.2f, 0.0f, 0.0f },
        { gv3(-1.0f,-1.0f, 1.0f), gv3(0.0f, 1.0f, 0.0f), 0.5f, 1.0f, 500.0f, 0.0f, 0.0f, 0.0f },
        { gv3( 1.5f, 1.0f,-1.0f), gv3(0.0f, 1.0f, 0.0f), 0.5f, 1.0f, 500.0f, 1.0f, 0.0f, 0.0f },
        { gv3(-1.5f, 1.0f,-1.0f), gv3(1.0f, 1.0f, 0.0f), 0.5f, 1.0f, 500.0f, 1.0f, 0.0f, 0.0f },
        { gv3( 0.5f,-0.2f, 2.5f), gv3(0.1f, 0.1f, 0.1f), 0.5f, 1.0f, 1.5f, 1.0f, 1.5f, 0.0f },
        { gv3( 1.0f,-1.5f, 0.0f), gv3(1.0f, 0.0f, 0.0f), 0.5f, 1.0f, 500.0f, 0.0f, 0.0f, 0.0f },
    };
    glGenBuffers(1, &sphereBuf);
    glBindBuffer(GL_TEXTURE_BUFFER, sphereBuf);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(Sphere) * spheres.size(), &spheres[0], GL_STATIC_DRAW);
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
        setInt(shader, "sphereNum", spheres.size());
        setInt(shader, "width", ScrWidth);
        setInt(shader, "height", ScrHeight);
        setVec3(shader, "ambient", gv3(0.02f));
        setVec3(shader, "viewPos", camera.Position);
        float ratio = (float)ScrWidth / ScrHeight;
        setVec3(shader, "r00", glm::normalize(camera.Front * 2.0f - camera.Up - camera.Right * ratio));
        setVec3(shader, "r01", glm::normalize(camera.Front * 2.0f - camera.Up + camera.Right * ratio));
        setVec3(shader, "r10", glm::normalize(camera.Front * 2.0f + camera.Up - camera.Right * ratio));
        setVec3(shader, "r11", glm::normalize(camera.Front * 2.0f + camera.Up + camera.Right * ratio));
        setVec3(shader, "dirLight.direction", gv3(0.0f, 1.0, 0.0f));
        setVec3(shader, "dirLight.color", gv3(1.0f, 1.0f, 1.0f));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCube);
        setInt(shader, "box", 1);
        glDispatchCompute((ScrWidth + 7)/ 16, (ScrHeight + 7)/ 16, 1);
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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(YAW_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(YAW_RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        camera.ProcessKeyboard(PITCH_UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        camera.ProcessKeyboard(PITCH_DOWN, deltaTime);
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
