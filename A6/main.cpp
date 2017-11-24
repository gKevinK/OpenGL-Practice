#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <stb_image.h>
#include <iostream>
#include "shader.h"
#include "shaders.glsl.hpp"
#include "fps_camera.h"

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
Camera camera = Camera(gv3(0.0f, 0.0f, 5.0f));

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

    unsigned int waterVAO, waterVBO;

    // Skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxCube;
    float skyboxVertices[] = {         
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
    };

    std::vector<std::string> cubePaths =
    {
        "resource\\right.jpg",
        "resource\\left.jpg",
        "resource\\top.jpg",
        "resource\\bottom.jpg",
        "resource\\back.jpg",
        "resource\\front.jpg"
    };
    skyboxCube = loadCubeMap(cubePaths);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    // Skybox Build
    //unsigned int skyboxCube;
    unsigned int skybox2TexIds[] = {
        loadTexture("resource\\SkyBox\\SkyBox0.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox1.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox2.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox3.bmp", GL_CLAMP_TO_EDGE),
        loadTexture("resource\\SkyBox\\SkyBox4.bmp", GL_CLAMP_TO_EDGE),
        //loadTexture("resource\\SkyBox\\SkyBox5.bmp")
    };
    float skybox2Vertices[] = {
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         1.0f, 2.0f, -1.0f, 1.0f, 1.0f,
         1.0f, 2.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 2.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
        1.0f, 2.0f,  1.0f, 1.0f, 1.0f,
        1.0f, 2.0f,  1.0f, 1.0f, 1.0f,
        1.0f, 2.0f, -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

         1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 2.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 2.0f, 1.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 2.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 2.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

        -1.0f, 2.0f, -1.0f, 0.0f, 0.0f,
         1.0f, 2.0f, -1.0f, 1.0f, 0.0f,
         1.0f, 2.0f,  1.0f, 1.0f, 1.0f,
         1.0f, 2.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, 2.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, 2.0f, -1.0f, 0.0f, 0.0f,
    };
    //int skybox2Texs[] = { 0, 1, 2, 3, 4 };
    unsigned int skybox2VAO, skybox2VBO;
    glGenVertexArrays(1, &skybox2VAO);
    glGenBuffers(1, &skybox2VBO);
    glBindVertexArray(skybox2VAO);
    glBindBuffer(GL_ARRAY_BUFFER, skybox2VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox2Vertices), &skybox2Vertices, GL_STATIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), sizeof(skybox2Texs), &skybox2Texs);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 1 * sizeof(int), (void *)sizeof(skybox2Vertices));
    glBindVertexArray(0);


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


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_FRAMEBUFFER_SRGB);

        glDepthMask(GL_FALSE);
        //glUseProgram(skyboxShader);
        glUseProgram(skybox2Shader);
        setMat4(skyboxShader, "view", gm4(gm3(camera.GetViewMatrix())));
        gm4 proj = glm::perspective(camera.Zoom, (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f);
        setMat4(skyboxShader, "proj", proj);
        //glBindVertexArray(skyboxVAO);
        //glActiveTexture(GL_TEXTURE0);
        //setInt(skyboxShader, "skybox", 0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCube);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(skybox2VAO);
        glActiveTexture(GL_TEXTURE0);
        setInt(terrainShader, "tex", 0);
        for (int i = 0; i < 5; i++) {
            glBindTexture(GL_TEXTURE_2D, skybox2TexIds[i]);
            glDrawArrays(GL_TRIANGLES, i * 6, 6);
        }
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

        glUseProgram(terrainShader);
        glBindVertexArray(terrainVAO);
        setVec3(terrainShader, "viewPos", camera.Position);
        setMat4(terrainShader, "model", glm::translate(glm::scale(gm4(1.0f), gv3(5.0f, 5.0f, 5.0f)), gv3(-0.5f, 0.0f, 0.0f)));
        setMat4(terrainShader, "view", camera.GetViewMatrix());
        setMat4(terrainShader, "proj", proj);
        setFloat(terrainShader, "base", -0.1f);
        setFloat(terrainShader, "scale", 0.2f);
        glActiveTexture(GL_TEXTURE1);
        setInt(terrainShader, "heightMap", 1);
        glBindTexture(GL_TEXTURE_2D, terrainHeight);
        glActiveTexture(GL_TEXTURE2);
        setInt(terrainShader, "textureMap", 2);
        glBindTexture(GL_TEXTURE_2D, terrainTex);
        glActiveTexture(GL_TEXTURE3);
        setInt(terrainShader, "detailMap", 3);
        glBindTexture(GL_TEXTURE_2D, terrainDetail);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawArrays(GL_PATCHES, 0, terrainVertices.size() / 4);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
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
        if (camera.Position.y > 0.1)
            camera.ProcessKeyboard(DOWN, deltaTime);
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