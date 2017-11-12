#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include "fps_camera.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 12.0f));
float scale = 4.0;
bool flat = false;

int main(int argc, char ** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "C-1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		0.0f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-0.5, 0.2f, 0.0f,  0.0f, 0.0f, 1.0f,

		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
	   -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
	   -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
	    0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
	};
	int rect_indice[] = {
		3, 4, 6, 5, 6, 4,
	};

	unsigned int shader = LoadShaderProgram("main.vert.glsl", "main.frag.glsl");
	unsigned int VBO1, VAO1, EBO2;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &EBO2);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indice), rect_indice, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	std::cout << "Press 'Q' to switch flat mode. Use mouse wheel to scale." << std::endl;
	std::cout << "Press 'Esc' to exit." << std::endl;

	float angle = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		angle += 0.5f * deltaTime;
		if (angle > 360.0f)
			angle -= 360.0f;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader);

		setBool(shader, "Flat", flat);
		setMat4(shader, "view", glm::scale(glm::mat4(1), glm::vec3(glm::pow(2.0f, -scale) * SCR_HEIGHT / SCR_WIDTH, glm::pow(2.0f, -scale), 1.0f)));

		glm::mat4 t1 = glm::translate(glm::mat4(1), glm::vec3(5.0f, 5.0f, 0.0f));
		glm::mat4 r = glm::rotate(glm::mat4(1), -angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 t2 = glm::translate(glm::mat4(1), glm::vec3(-5.0f, -5.0f, 0.0f));
		glm::mat4 sc = glm::scale(glm::mat4(1), glm::vec3(3.0f, 3.0f, 1.0f));
		glm::mat4 model_t = t1 * r * t2 * sc;
		setMat4(shader, "model", model_t);
		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		t1 = glm::translate(glm::mat4(1), glm::vec3(-3.0f, -3.0f, 0.0f));
		r = glm::rotate(t1, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		sc = glm::scale(r, glm::vec3(2.0f, 2.0f, 1.0f));
		setMat4(shader, "model", sc);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &EBO2);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

bool q_pressed = false;
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (!q_pressed && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		flat = !flat;
	}
	q_pressed = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scale += (float)yoffset / 20;
}
