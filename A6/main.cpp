#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <stb_image.h>
#include <iostream>
#include "shader.h"
#include "shaders.glsl.hpp"
#include "fps_camera.h"

#include "skybox2.hpp"
#include "window.hpp"

#define gv3 glm::vec3
#define gm3 glm::mat3
#define gm4 glm::mat4

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposd, double yposd);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const std::string & path, GLint wrap = GL_REPEAT);
unsigned int loadCubeMap(const std::vector<std::string> & paths);

unsigned int ScrWidth = 1000;
unsigned int ScrHeight = 800;
float lastX = ScrWidth / 2.0f;
float lastY = ScrHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera = Camera(gv3(0.0f, 1.0f, 5.0f));

int main(int argc, char ** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(ScrWidth, ScrHeight, "B-7", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

#ifdef _DEBUG
    unsigned int windowShader = loadShaderProgram("window.vert.glsl", "texture.frag.glsl");
    unsigned int terrainShader = loadShaderProgram("terrain.vert.glsl", "terrain.tesc.glsl", "terrain.tese.glsl", "terrain.frag.glsl");
    unsigned int waterShader = loadShaderProgram("water.vert.glsl", "water.tesc.glsl", "water.tese.glsl", "water.frag.glsl");
    unsigned int skyboxShader = loadShaderProgram("skybox.vert.glsl", "skybox.frag.glsl");
    unsigned int skybox2Shader = loadShaderProgram("skybox2.vert.glsl", "skybox2.frag.glsl");
#else
    unsigned int terrainShader = loadShaderProgramS(terrain_vert_glsl, terrain_tesc_glsl, terrain_tese_glsl, terrain_frag_glsl);
    unsigned int waterShader = loadShaderProgramS(water_vert_glsl, water_tesc_glsl, water_tese_glsl, water_frag_glsl);
    unsigned int skyboxShader = loadShaderProgramS(skybox_vert_glsl, skybox_frag_glsl);
    unsigned int skybox2Shader = loadShaderProgramS(skybox2_vert_glsl, skybox2_frag_glsl);
#endif // _DEBUG

    // Window
    Window wind = Window(-1.0f, -1.0f, 0.8f, 0.8f);
    wind.Init();
    Window wind2 = Window(-0.2f, -1.0f, 0.8f, 0.8f);
    wind2.Init();

    // Terrain
    unsigned int terrainVAO, terrainVBO, terrainHeight, terrainTex, terrainDetail;
    std::vector<float> terrainVertices;
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            float fi = (float)i;
            float fj = (float)j;
            float vertices[] = {
                fi, fj, fi, fj,
                fi, fj + 1, fi, fj + 1,
                fi + 1, fj, fi + 1, fj,
                fi + 1, fj + 1, fi + 1, fj + 1 };
            for (int k = 0; k < 16; k++) {
                terrainVertices.push_back(vertices[k] / 20.0f);
            }
        }
    }
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glBindVertexArray(terrainVAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * terrainVertices.size(), &terrainVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    terrainTex = loadTexture("resource/terrain-texture3.bmp");
    terrainHeight = loadTexture("resource/heightmap.bmp");
    terrainDetail = loadTexture("resource/detail.bmp");

    // Skybox
    //unsigned int skyboxVAO, skyboxVBO, skyboxCube;
    //float skyboxVertices[] = {
    //    -1.0f,  1.0f, -1.0f,
    //    -1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,
    //     1.0f,  1.0f, -1.0f,
    //    -1.0f,  1.0f, -1.0f, //
    //    -1.0f, -1.0f,  1.0f,
    //    -1.0f, -1.0f, -1.0f,
    //    -1.0f,  1.0f, -1.0f,
    //    -1.0f,  1.0f, -1.0f,
    //    -1.0f,  1.0f,  1.0f,
    //    -1.0f, -1.0f,  1.0f, //
    //    1.0f, -1.0f, -1.0f,
    //    1.0f, -1.0f,  1.0f,
    //    1.0f,  1.0f,  1.0f,
    //    1.0f,  1.0f,  1.0f,
    //    1.0f,  1.0f, -1.0f,
    //    1.0f, -1.0f, -1.0f, //
    //    -1.0f, -1.0f,  1.0f,
    //    -1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f, -1.0f,  1.0f,
    //    -1.0f, -1.0f,  1.0f, //
    //    -1.0f,  1.0f, -1.0f,
    //     1.0f,  1.0f, -1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //    -1.0f,  1.0f,  1.0f,
    //    -1.0f,  1.0f, -1.0f, //
    //    -1.0f, -1.0f, -1.0f,
    //    -1.0f, -1.0f,  1.0f,
    //     1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,
    //    -1.0f, -1.0f,  1.0f,
    //     1.0f, -1.0f,  1.0f,
    //};
    //std::vector<std::string> cubePaths =
    //{
    //    "resource\\right.jpg",
    //    "resource\\left.jpg",
    //    "resource\\top.jpg",
    //    "resource\\bottom.jpg",
    //    "resource\\back.jpg",
    //    "resource\\front.jpg"
    //};
    //skyboxCube = loadCubeMap(cubePaths);
    //glGenVertexArrays(1, &skyboxVAO);
    //glGenBuffers(1, &skyboxVBO);
    //glBindVertexArray(skyboxVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    //glBindVertexArray(0);

    // Skybox
    Skybox2 skybox2;
    skybox2.Init();
    skybox2.TexIds = std::vector<unsigned int> {
        loadTexture("resource\\SkyBox\\SkyBox0.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox1.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox2.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox3.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox4.bmp", GL_CLAMP_TO_EDGE),
        //loadTexture("resource\\SkyBox\\SkyBox5.bmp")
    };

    float waterVertices[] = {
        -100.0f, -100.0f,
         100.0f, -100.0f,
        -100.0f,  100.0f,
         100.0f,  100.0f,
    };
    unsigned int waterVAO, waterVBO, waterTex;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), &waterVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindVertexArray(0);
    waterTex = loadTexture("resource\\SkyBox\\SkyBox5.bmp");

    {
    //    glViewport(0, 0, 256, 256);
    //    unsigned int frameBuffer;
    //    glGenFramebuffers(1, &frameBuffer);
    //    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    //    unsigned int texColorBuffer, texDepthBuffer;
    //    glGenTextures(1, &texColorBuffer);
    //    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    //    // Render the skybox

    //    glGenTextures(1, &skyboxCube);
    //    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCube);
    //    for (int i = 0; i < 6; i++)
    //    {
    //        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
    //            0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    }
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //    glViewport(0, 0, ScrWidth, ScrHeight);
    }

    unsigned int frameBuffer, texColorBuffer, texDepthBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScrWidth, ScrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    glGenTextures(1, &texDepthBuffer);
    glBindTexture(GL_TEXTURE_2D, texDepthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, ScrWidth, ScrHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texDepthBuffer, 0);
    //glGenRenderbuffers(1, &RBO);
    //glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ScrWidth, ScrHeight);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);

        gv3 worldUp = gv3(0.0f, 1.0f, 0.0f);
        gv3 reflectPos = glm::reflect(camera.Position, worldUp);
        gm4 view = camera.GetViewMatrix();
        gm4 proj = glm::perspective(camera.Zoom, (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f);
        gm4 reflectView = glm::lookAt(reflectPos, reflectPos + glm::reflect(camera.Front, worldUp), worldUp);
        gm4 reflectProj = glm::perspective(30.0f, (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f);
        
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glUseProgram(skybox2Shader);
        setMat4(skyboxShader, "view", gm4(gm3(reflectView)));
        setMat4(skyboxShader, "proj", reflectProj);
        skybox2.Draw(skybox2Shader);
        glDepthMask(GL_TRUE);

        glUseProgram(terrainShader);
        glBindVertexArray(terrainVAO);
        setVec3(terrainShader, "viewPos", reflectPos);
        setMat4(terrainShader, "model", glm::translate(glm::scale(gm4(1.0f), gv3(5.0f, 5.0f, 5.0f)), gv3(-0.5f, 0.0f, 0.0f)));
        setMat4(terrainShader, "view", reflectView);
        setMat4(terrainShader, "proj", reflectProj);
        setFloat(terrainShader, "base", -0.06f);
        setFloat(terrainShader, "scale", 0.2f);
        setTexture2D(terrainShader, "heightMap", 1, terrainHeight);
        setTexture2D(terrainShader, "textureMap", 2, terrainTex);
        setTexture2D(terrainShader, "detailMap", 3, terrainDetail);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawArrays(GL_PATCHES, 0, terrainVertices.size() / 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDepthMask(GL_FALSE);
        glUseProgram(skybox2Shader);
        setMat4(skyboxShader, "view", gm4(gm3(camera.GetViewMatrix())));
        setMat4(skyboxShader, "proj", proj);
        skybox2.Draw(skybox2Shader);
        glDepthMask(GL_TRUE);

        glUseProgram(terrainShader);
        glBindVertexArray(terrainVAO);
        setVec3(terrainShader, "viewPos", camera.Position);
        setMat4(terrainShader, "model", glm::translate(glm::scale(gm4(1.0f), gv3(5.0f, 5.0f, 5.0f)), gv3(-0.5f, 0.0f, 0.0f)));
        setMat4(terrainShader, "view", camera.GetViewMatrix());
        setMat4(terrainShader, "proj", proj);
        setFloat(terrainShader, "base", -0.06f);
        setFloat(terrainShader, "scale", 0.2f);
        setTexture2D(terrainShader, "heightMap", 1, terrainHeight);
        setTexture2D(terrainShader, "textureMap", 2, terrainTex);
        setTexture2D(terrainShader, "detailMap", 3, terrainDetail);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawArrays(GL_PATCHES, 0, terrainVertices.size() / 4);
        glBindVertexArray(0);

        glUseProgram(waterShader);
        glBindVertexArray(waterVAO);
        setFloat(waterShader, "time", currentFrame);
        setVec3(waterShader, "viewPos", camera.Position);
        setMat4(waterShader, "model", gm4(1.0f));
        setMat4(waterShader, "view", camera.GetViewMatrix());
        setMat4(waterShader, "proj", proj);
        setMat4(waterShader, "reflViewMat", reflectView);
        setMat4(waterShader, "reflProjMat", reflectProj);
        setTexture2D(waterShader, "reflColor", 1, texColorBuffer);
        setTexture2D(waterShader, "reflDepth", 2, texDepthBuffer);
        setTexture2D(waterShader, "waterTex", 3, waterTex);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawArrays(GL_PATCHES, 0, 4);
        glBindVertexArray(0);

        glUseProgram(windowShader);
        setTexture2D(windowShader, "tex", 1, texColorBuffer);
        wind.Draw(windowShader);
        setTexture2D(windowShader, "tex", 2, texDepthBuffer);
        wind2.Draw(windowShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
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
    if (camera.Position.y < 0.1) {
        camera.Position.y = 0.1;
    }
}

void mouse_callback(GLFWwindow* window, double xposd, double yposd)
{
    float xpos = (float)xposd;
    float ypos = (float)yposd;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    //if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(const std::string & path, GLint wrap)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
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