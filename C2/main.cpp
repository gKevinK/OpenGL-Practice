#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include "fps_camera.h"
#include "shader.h"
#include "light.h"
#include "vertex.h"
#include "text.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposd, double yposd);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
std::string to_string(float input, int precision);

unsigned int ScrWidth = 1000;
unsigned int ScrHeight = 800;
float lastX = ScrWidth / 2.0f;
float lastY = ScrHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

const int selection_count = 7;
int selection = 1;
float ambient = 0.2f;
float diffuse = 0.8f;
float specular = 1.0f;
float shininess = 6.0f;
float dir_color = 0.5f;
float point_color = 1.0f;
float spot_color = 1.0f;

int main(int argc, char ** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(ScrWidth, ScrHeight, "C-2", NULL, NULL);
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
	TextRenderer textr;
	textr.Init("C:\\Windows\\Fonts\\arial.ttf", 24);

	Vertex vertex[] = {
		Vertex(1.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f),
		Vertex(1.0f, 0.0f,-1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, 0.0f),
		Vertex(-1.0f,0.0f,-1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f),
		Vertex(-1.0f,0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f),
		Vertex(0.0f, glm::sqrt(2.0f), 0.0f, 0, 0, 0,  1.0f, 1.0f, 1.0f),
	};
	Vertex vertices[] = {
		vertex[0], vertex[2], vertex[1],
		vertex[2], vertex[0], vertex[3],

		vertex[0], vertex[1], vertex[4],
		vertex[1], vertex[2], vertex[4],
		vertex[2], vertex[3], vertex[4],
		vertex[3], vertex[0], vertex[4],
	};
	for (int i = 6; i < 18; i += 3) {
		glm::vec3 norm = glm::normalize(glm::cross(vertices[i + 1].Position - vertices[i].Position,
												   vertices[i + 2].Position - vertices[i].Position));
		vertices[i + 2].Normal = norm;
		vertices[i + 1].Normal = norm;
		vertices[i].Normal = norm;
	}
	DirLight dirLight(glm::vec3(1.0f, 5.0f, 2.0f));
	std::vector<PointLight> pointLights = {
		PointLight(glm::vec3(5.0f, 5.0f, 3.0f)),
	};
	SpotLight spotLight = SpotLight(camera.Position, camera.Front);
	
	unsigned int shader = LoadShaderProgram("main.vert.glsl", "main.frag.glsl");
	unsigned int textShader = LoadShaderProgram("text.vert.glsl", "text.frag.glsl");
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

	std::cout << "Press 'WASDRFQE' to explore:" << std::endl
		<< "    W: Front    S: Back     A: Left     D: Right" << std::endl
		<< "    R: Up       F: Down     Q: Yaw left E: Yaw right" << std::endl
		<< std::endl
		<< "Press arrow keys to change the configuration." << std::endl;

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);
		setVec3(shader, "viewPos", camera.Position);
		setVec3(shader, "ambient", glm::vec3(ambient));
		setFloat(shader, "material.diffuse", diffuse);
		setFloat(shader, "material.specular", specular);
		setFloat(shader, "material.shininess", std::pow(2, shininess));
		dirLight.Color = glm::vec3(dir_color);
		dirLight.SetUniform(shader, "dirLight");
		setInt(shader, "pointLightNum", (int)pointLights.size());
		for (int i = 0; i < pointLights.size(); i++) {
			pointLights[i].Color = glm::vec3(point_color);
			pointLights[i].SetUniform(shader, std::string("pointLights[") + std::to_string(i) + "]");
		}
		spotLight.Color = glm::vec3(spot_color);
        spotLight.Position = camera.Position;
        spotLight.Direction = camera.Front;
		spotLight.SetUniform(shader, "spotLight");
		
		setMat4(shader, "model", glm::translate(glm::mat4(1), glm::vec3(1.0f, 2.0f, 3.0f)));
		setMat4(shader, "view", camera.GetViewMatrix());
		glm::mat4 pers = glm::perspective(glm::radians(camera.Zoom), (float)ScrWidth / (float)ScrHeight, 0.1f, 100.0f);
		setMat4(shader, "proj", pers);
		glDrawArrays(GL_TRIANGLES, 0, 18);

		glm::vec3 text_green(0.5f, 0.9f, 0.4f);
		glm::vec3 text_white(1.0f, 1.0f, 1.0f);
		glUseProgram(textShader);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		setMat4(textShader, "model", glm::mat4(1));
		setMat4(textShader, "view", glm::mat4(1));
		setMat4(textShader, "proj", glm::ortho(0.0f, (float)ScrWidth, 0.0f, (float)ScrHeight));
		textr.RenderText(textShader, "Ambient",              20.0f, 140.0f, 0.8f, text_green);
		textr.RenderText(textShader, to_string(ambient, 3), 120.0f, 140.0f, 0.8f, selection == 1 ? text_white : text_green);
		textr.RenderText(textShader, "Diffuse",              20.0f, 120.0f, 0.8f, text_green);
		textr.RenderText(textShader, to_string(diffuse, 3), 120.0f, 120.0f, 0.8f, selection == 2 ? text_white : text_green);
		textr.RenderText(textShader, "Specular",             20.0f, 100.0f, 0.8f, text_green);
		textr.RenderText(textShader, to_string(specular, 3),120.0f, 100.0f, 0.8f, selection == 3 ? text_white : text_green);
        textr.RenderText(textShader, "Shininess",                           20.0f,  80.0f, 0.8f, text_green);
        textr.RenderText(textShader, to_string(std::pow(2, shininess), 1), 120.0f,  80.0f, 0.8f, selection == 4 ? text_white : text_green);
        textr.RenderText(textShader, "DirLight",                 20.0f,  60.0f, 0.8f, text_green);
        textr.RenderText(textShader, to_string(dir_color, 3),   120.0f,  60.0f, 0.8f, selection == 5 ? text_white : text_green);
        textr.RenderText(textShader, "PointLight",               20.0f,  40.0f, 0.8f, text_green);
        textr.RenderText(textShader, to_string(point_color, 3), 120.0f,  40.0f, 0.8f, selection == 6 ? text_white : text_green);
        textr.RenderText(textShader, "SpotLight",                20.0f,  20.0f, 0.8f, text_green);
        textr.RenderText(textShader, to_string(spot_color, 3),  120.0f,  20.0f, 0.8f, selection == 7 ? text_white : text_green);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//textr.~TextRenderer();
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

bool up_pressed = false;
bool down_pressed = false;
void process_config(int selection, float delta)
{
	switch (selection)
	{
	case 1:
		ambient += delta;
		if (ambient > 1.0f) ambient = 1.0f;
		if (ambient < 0.0f) ambient = 0.0f;
		break;
	case 2:
		diffuse += delta;
		if (diffuse > 1.0f) diffuse = 1.0f;
		if (diffuse < 0.0f) diffuse = 0.0f;
		break;
	case 3:
		specular += delta;
		if (specular > 1.0f) specular = 1.0f;
		if (specular < 0.0f) specular = 0.0f;
		break;
	case 4:
        shininess += delta * 5;
        if (shininess > 8.0f) shininess = 8.0f;
        if (shininess < 4.0f) shininess = 4.0f;
        break;
    case 5:
		dir_color += delta;
		if (dir_color > 2.0f) dir_color = 2.0f;
		if (dir_color < 0.0f) dir_color = 0.0f;
		break;
	case 6:
		point_color += delta * 3;
		if (point_color > 2.0f) point_color = 2.0f;
		if (point_color < 0.0f) point_color = 0.0f;
		break;
	case 7:
		spot_color += delta * 3;
		if (spot_color > 2.0f) spot_color = 2.0f;
		if (spot_color < 0.0f) spot_color = 0.0f;
		break;
	default:
		break;
	}
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
	if (!up_pressed && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		selection -= 1;
		if (selection == 0)
			selection += selection_count;
	}
	up_pressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	if (!down_pressed && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		selection += 1;
		if (selection > selection_count)
			selection -= selection_count;
	}
	down_pressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		process_config(selection, -deltaTime * 0.5);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		process_config(selection, deltaTime * 0.5);
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
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

std::string to_string(float input, int precision)
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(precision) << input;
	return stream.str();
}
