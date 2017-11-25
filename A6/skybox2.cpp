#include "skybox2.hpp"

const float Skybox2::skybox2Vertices[] {
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

Skybox2::~Skybox2()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Skybox2::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox2Vertices), &skybox2Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void Skybox2::Draw(unsigned int shader)
{
    glDepthMask(GL_FALSE);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader, "tex"), 0);
    for (int i = 0; i < 5; i++) {
        glBindTexture(GL_TEXTURE_2D, TexIds[i]);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}
