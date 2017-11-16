#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include "shader.h"
#include "view_camera.h"
#include "light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const std::string & path);
void loadBezierData(float vertices[], const std::string & path);

unsigned int ScrWidth = 1000;
unsigned int ScrHeight = 800;
float lastX = ScrWidth / 2.0f;
float lastY = ScrHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
bool needReload = false;
bool lineMode = false;
int level = 4;

int main(int argc, char ** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(ScrWidth, ScrHeight, "C-2", NULL, NULL);
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

    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        2.0f, 0.0f, 0.0f,
        3.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        2.0f, 1.0f, 0.0f,
        3.0f, 1.0f, 0.0f,
        0.0f, 2.0f, 0.0f,
        1.0f, 2.0f, 0.0f,
        2.0f, 2.0f, 0.0f,
        3.0f, 2.0f, 0.0f,
        0.0f, 3.0f, 0.0f,
        1.0f, 3.0f, 0.0f,
        2.0f, 3.0f, 0.0f,
        3.0f, 3.0f, 0.0f,
    };
    loadBezierData(vertices, "resource\\bezier.txt");

    DirLight dirLight(glm::vec3(1.0f, 5.0f, 2.0f));
    std::vector<PointLight> pointLights = {
        PointLight(glm::vec3(3.0f, 3.0f, 3.0f)),
    };
    //SpotLight spotLight = SpotLight(camera.Position, glm::vec3(0.0f, 0.0f, 0.0f));

    unsigned int shader = loadShaderProgram("main.vert.glsl", "main.tesc.glsl", "main.tese.glsl", "main.frag.glsl");
    unsigned VAO, VBO, Texture;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)NULL);
    glEnableVertexAttribArray(0);
    Texture = loadTexture("resource\\texture.png");
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout
        << "Press 'W' 'A' 'S' 'D' 'R' 'F' to move the camera" << std::endl << std::endl;
    std::cout
        << "Press 'Space' to reload the Bezier surface data from file" << std::endl
        << "Press 'Z' and 'X' to decrease or increase tessellation level" << std::endl
        << "Press 'C' to switch line mode" << std::endl;
    std::cout << "Level: " << level << std::endl;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);
        if (needReload) {
            loadBezierData(vertices, "resource\\bezier.txt");
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            needReload = false;
        }
        setFloat(shader, "outerLevel", (float)level);
        setFloat(shader, "innerLevel", (float)level);
        setMat4(shader, "model", glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.5f, 0.0f)));
        setMat4(shader, "view", camera.GetViewMatrix());
        setMat4(shader, "proj", glm::perspective(glm::radians(45.0f), (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f));
        setInt(shader, "tex", 0);

        setVec3(shader, "viewPos", camera.Position);
        setVec3(shader, "ambient", glm::vec3(0.2f));
        setFloat(shader, "material.diffuse", 0.8f);
        setFloat(shader, "material.specular", 1.0f);
        setFloat(shader, "material.shininess", std::pow(2, 64));
        dirLight.SetUniform(shader, "dirLight");
        setInt(shader, "pointLightNum", (int)pointLights.size());
        for (int i = 0; i < pointLights.size(); i++) {
            pointLights[i].SetUniform(shader, std::string("pointLights[") + std::to_string(i) + "]");
        }
        
        if (lineMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, 16);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    ScrWidth = width;
    ScrHeight = height;
    glViewport(0, 0, width, height);
}

bool space_pressed = false;
bool c_pressed = false;
bool z_pressed = false;
bool x_pressed = false;
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessInput(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessInput(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessInput(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessInput(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessInput(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        camera.ProcessInput(DOWN, deltaTime);
    if (!space_pressed && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        needReload = true;
    space_pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (!c_pressed && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        lineMode = !lineMode;
    c_pressed = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
    if (!z_pressed && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && level > 1) {
        if (level <= 2)
            level -= 1;
        else
            level -= 2;
        std::cout << "Level: " << (int)level << std::endl;
    }
    z_pressed = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
    if (!x_pressed && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && level < 40) {
        if (level < 20)
            level += 1;
        else
            level += 2;
        std::cout << "Level: " << (int)level << std::endl;
    }
    x_pressed = glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS;
}

unsigned int loadTexture(const std::string & path)
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

void loadBezierData(float vertices[], const std::string & path)
{
    std::ifstream fileStream;
    std::string code;
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fileStream.open(path);
        for (int i = 0; i < 16; i++) {
            float x, y, z;
            fileStream >> x >> y >> z;
            vertices[i * 3] = x;
            vertices[i * 3 + 1] = y;
            vertices[i * 3 + 2] = z;
        }
        fileStream.close();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cout << path << std::endl;
    }
}
