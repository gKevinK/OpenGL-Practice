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
#include "model.h"
#include "text.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposd, double yposd);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void loadObject(const std::string & path);

unsigned int ScrWidth = 1000;
unsigned int ScrHeight = 800;
float lastX = ScrWidth / 2.0f;
float lastY = ScrHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
int drawMode = 0;
bool light = true;
glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    TextRenderer textr;
    textr.Init("C:\\Windows\\Fonts\\arial.ttf", 24);

    std::vector<float> vertices = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
    };

    DirLight dirLight(glm::vec3(0.0f, 5.0f, 0.0f));
    std::vector<PointLight> pointLights = {
        PointLight(glm::vec3(3.0f, 3.0f, 3.0f)),
    };
    //SpotLight spotLight = SpotLight(camera.Position, glm::vec3(0.0f, 0.0f, 0.0f));

    unsigned int shader = loadShaderProgram("main.vert.glsl", "main.frag.glsl");
    unsigned int textsh = loadShaderProgram("text.vert.glsl", "text.frag.glsl");
    glUseProgram(shader);
    Mesh mesh;
    mesh.LoadFile("resource\\eight.uniform.obj");
    mesh.SetMesh();
    mesh.SetData(true);

    std::cout
        << "Press 'W' 'A' 'S' 'D' 'R' 'F' to move the camera" << std::endl << std::endl;
    //std::cout
    //    << "Press 'Space' to reload the Bezier surface data from file" << std::endl
    //    << "Press 'Z' and 'X' to decrease or increase tessellation level" << std::endl
    //    << "Press 'C' to switch line mode" << std::endl << std::endl;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(mesh.VAO);
        setMat4(shader, "model", glm::mat4(1.0f));
        setMat4(shader, "view", camera.GetViewMatrix());
        setMat4(shader, "proj", glm::perspective(glm::radians(45.0f), (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f));

        setVec3(shader, "color", color);
        setVec3(shader, "viewPos", camera.Position);
        setVec3(shader, "ambient", glm::vec3(0.2f));
        setFloat(shader, "material.diffuse", 0.8f);
        setFloat(shader, "material.specular", 1.0f);
        setFloat(shader, "material.shininess", 64.0f);
        dirLight.SetUniform(shader, "dirLight");
        setInt(shader, "pointLightNum", (int)pointLights.size());
        for (int i = 0; i < pointLights.size(); i++) {
            pointLights[i].SetUniform(shader, std::string("pointLights[") + std::to_string(i) + "]");
        }

        switch (drawMode)
        {
        case 0:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            setBool(shader, "useLight", false);
            mesh.Draw(GL_TRIANGLES);
            break;
        case 1:
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            setBool(shader, "useLight", false);
            glPointSize(2.0f);
            mesh.Draw(GL_POINTS);
            break;
        case 2:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            setBool(shader, "useLight", light);
            mesh.Draw(GL_TRIANGLES);
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            setBool(shader, "useLight", false);
            mesh.Draw(GL_TRIANGLES);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            setBool(shader, "useLight", light);
            mesh.Draw(GL_TRIANGLES);
            break;
        default:
            break;
        }
        
        glm::vec3 text_green(0.5f, 0.9f, 0.4f);
        glUseProgram(textsh);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setMat4(textsh, "proj", glm::ortho(0.0f, (float)ScrWidth, 0.0f, (float)ScrHeight));
        textr.RenderText(textsh, "Ambient", 20.0f, 140.0f, 0.8f, text_green);
        textr.RenderText(textsh, to_string(ambient, 3), 120.0f, 140.0f, 0.8f, selection == 1 ? text_white : text_green);
        textr.RenderText(textsh, "Diffuse", 20.0f, 120.0f, 0.8f, text_green);
        textr.RenderText(textsh, to_string(diffuse, 3), 120.0f, 120.0f, 0.8f, selection == 2 ? text_white : text_green);

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

bool z_pressed = false;
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
    if (!z_pressed && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        drawMode = (drawMode + 1) % 4;
    z_pressed = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
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
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

void loadObject(const std::string & path)
{

}
