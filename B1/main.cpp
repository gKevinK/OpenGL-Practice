#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <stb_image.h>
#include <iostream>
#include <list>
#include <algorithm>
#include "shader.h"

class Star
{
public:
    glm::vec3 Dir;
    glm::vec3 Color;
    float GenTime;
    float CurrentTime;
    int Mode;

    Star() {}

    Star(glm::vec3 direction, glm::vec3 color, float genTime, int mode)
        : Dir(direction), Color(color), GenTime(genTime), Mode(mode) {}

    glm::vec3 Position()
    {
        if (Mode == 0)
            return Dir * (CurrentTime - GenTime);
        else if (Mode == 1)
            return Dir * glm::exp(CurrentTime - GenTime) / 5.0f;
        else if (Mode == 2)
            return Dir * glm::sqrt(CurrentTime - GenTime) * 4.0f;
        //else if (Mode == 3)
        //    return Dir * glm::log(CurrentTime - GenTime + 1.0f) * 2.5f;
    }

    glm::vec3 GetColor()
    {
        float life = CurrentTime - GenTime;
        if (life > 6.0f)
            return (8.0f - life) / 2.0f * Color;
        else
            return Color;
    }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const std::string & path);

unsigned int ScrWidth = 1000;
unsigned int ScrHeight = 800;
float lastX = ScrWidth / 2.0f;
float lastY = ScrHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int mode = 0;
int sign = 1;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, 1.0f, 1.0f,
       -0.5f,  0.5f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
    };
    std::list<Star> stars;
    stars.push_back(Star(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0, mode));

    unsigned int shader = loadShaderProgram("main.vert.glsl", "main.frag.glsl");
    unsigned VAO, VBO, Texture;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    Texture = loadTexture("resource\\Star.bmp");
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::cout << "Press 'Q' to switch the spiral." << std::endl
        << "Press 'W' to reverse." << std::endl << std::endl;
    std::cout << "Archimedean spiral" << std::endl;

    glm::vec3 lastDir = glm::vec3(1.0f, 0.0f, 0.0f);
    float lastTime = 0;
    float genDelta = 2.0f / 16;
    float angleDelta = -glm::radians(360.0f / 16);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        while (currentFrame - lastTime > genDelta) {
            lastTime += genDelta;
            lastDir = glm::mat3(glm::rotate(glm::mat4(1),
                sign * angleDelta, glm::vec3(0.0f, 0.0f, 1.0f))) * lastDir;
            float i = lastTime / genDelta / 16 - std::floorf(lastTime / genDelta / 16);
            float r = std::min(1.0f, std::max(0.0f, std::max(6 * i - 4, 2 - 6 * i))) * 0.8f + 0.2f;
            float g = std::min(1.0f, std::max(0.0f, std::min(6 * i - 0, 4 - 6 * i))) * 0.8f + 0.2f;
            float b = std::min(1.0f, std::max(0.0f, std::min(6 * i - 2, 6 - 6 * i))) * 0.8f + 0.2f;
            stars.push_back(Star(lastDir, glm::vec3(r, g, b), lastTime, mode));
            if (mode == 2)
                stars.push_back(Star(-lastDir, glm::vec3(r, g, b), lastTime, mode));
        }
        while (!stars.empty() && currentFrame - stars.front().GenTime > 8.0) {
            stars.pop_front();
        }

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);
        setMat4(shader, "proj", glm::ortho(-10.0f / ScrHeight * ScrWidth, 10.0f / ScrHeight * ScrWidth, -10.0f, 10.0f));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        for (Star & s : stars) {
            s.CurrentTime = currentFrame;
            setMat4(shader, "model", glm::translate(glm::mat4(1), s.Position()));
            setVec3(shader, "color", s.GetColor());
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

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

bool q_pressed = false;
bool w_pressed = false;
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (!q_pressed && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        mode = (mode + 1) % 3;
        std::cout << std::endl;
        switch (mode)
        {
        case 0:
            std::cout << "Archimedean spiral" << std::endl;
            break;
        case 1:
            std::cout << "Logarithmic spiral" << std::endl;
            break;
        case 2:
            std::cout << "Fermat's spiral" << std::endl;
            break;
        case 3:
            std::cout << "Some spiral" << std::endl;
            break;
        default:
            break;
        }
    }
    q_pressed = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
    if (!w_pressed && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        sign = -sign;
    }
    w_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
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
