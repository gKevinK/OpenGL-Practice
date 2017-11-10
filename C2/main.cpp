#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "fps_camera.h"
#include "shader.h"
#include "light.h"
#include "vertex.h"
#include "freetype.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposd, double yposd);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int ScrWidth = 800;
unsigned int ScrHeight = 800;
float lastX = ScrWidth / 2.0f;
float lastY = ScrHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

int main(int argc, char ** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(ScrWidth, ScrHeight, "C3", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	std::map<GLchar, Character> characters = genFTCharacters("C:\\Windows\\Fonts\\arial.ttf", 24);
	
	Vertex vertex[] = {
		Vertex(1.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f),
		Vertex(1.0f, 0.0f,-1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, 0.0f),
		Vertex(-1.0f,0.0f,-1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f),
		Vertex(-1.0f,0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f),
		Vertex(0.0f, glm::sqrt(2.0f), 0.0f, 0, 0, 0,  1.0f, 1.0f, 1.0f),
	};
	Vertex vertices[] = {
		vertex[0], vertex[2], vertex[1], vertex[2], vertex[0], vertex[3],

		vertex[0], vertex[1], vertex[4],

		vertex[1], vertex[2], vertex[4],

		vertex[2], vertex[3], vertex[4],

		vertex[3], vertex[0], vertex[4],
	};
	for (int i = 6; i < 18; i += 3) {
		glm::vec3 norm = glm::normalize(glm::cross(vertices[i + 1].Position - vertices[i].Position, vertices[i + 2].Position - vertices[i].Position));
		vertices[i + 2].Normal = norm;
		vertices[i + 1].Normal = norm;
		vertices[i].Normal = norm;
	}
	DirLight dirLight(glm::vec3(1.0f, 5.0f, 2.0f));
	std::vector<PointLight> pointLights = {
		PointLight(glm::vec3(5.0f, 5.0f, 3.0f)),
	};
	
	unsigned int shader = LoadShaderProgram("main.vert.glsl", "main.frag.glsl");
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//std::cout << "Something..." << std::endl;

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		setVec3(shader, "viewPos", camera.Position);
		setVec3(shader, "ambient", 0.2f, 0.2f, 0.2f);
		setFloat(shader, "material.diffuse", 0.8f);
		setFloat(shader, "material.specular", 1.0f);
		setFloat(shader, "material.shininess", 32.0f);
		dirLight.SetUniform(shader, "dirLight");
		setInt(shader, "pointLightNum", pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			pointLights[i].SetUniform(shader, std::string("pointLights[") + std::to_string(i) + "]");
		}
		SpotLight spotLight = SpotLight(camera.Position, camera.Front);
		spotLight.SetUniform(shader, "spotLight");
		
		setMat4(shader, "model", glm::translate(glm::mat4(1), glm::vec3(1.0f, 2.0f, 3.0f)));
		setMat4(shader, "view", camera.GetViewMatrix());
		glm::mat4 pers = glm::perspective(glm::radians(camera.Zoom),
			(float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f);
		setMat4(shader, "proj", pers);

		glDrawArrays(GL_TRIANGLES, 0, 18);

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
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}
