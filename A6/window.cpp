#include "window.hpp"

Window::Window(float x, float y, float w, float h)
{
    vertices = {
        x, y, 0.0f, 0.0f,
        x + w, y, 1.0f, 0.0f,
        x + w, y + h, 1.0f, 1.0f,
        x + w, y + h, 1.0f, 1.0f,
        x, y + h, 0.0f, 1.0f,
        x, y, 0.0f, 0.0f,
    };
}

void Window::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void Window::Draw(unsigned int shader)
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

