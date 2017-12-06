#include "window.hpp"

const float Window::vertices[]{
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
};

Window::Window(float wx, float wy, float ww, float wh)
    : x(wx), y(wy), w(ww), h(wh) {}

void Window::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindVertexArray(0);
}

void Window::Draw(unsigned int shader)
{
    glBindVertexArray(VAO);
    glUniform1f(glGetUniformLocation(shader, "x"), x);
    glUniform1f(glGetUniformLocation(shader, "y"), y);
    glUniform1f(glGetUniformLocation(shader, "w"), w);
    glUniform1f(glGetUniformLocation(shader, "h"), h);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

