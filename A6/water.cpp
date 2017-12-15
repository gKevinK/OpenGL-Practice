#include "water.hpp"

const float Range = 20.0f;

const float Water::vertices[] {
   -Range, -Range,
    Range, -Range,
   -Range,  Range,
    Range,  Range,
};

void Water::Init()
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

//void Water::Update(float x, float y, float z)
//{
//    float r = y > 5.0f ? y * 40.0f : 200.0f;
//    vertices = {
//        x - r, -z - r,
//        x + r, -z - r,
//        x - r, -z + r,
//        x + r, -z + r,
//    };
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
//    glBindVertexArray(0);
//}

void Water::Draw(unsigned int shader)
{
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader, "waterTex"), 1);
    glBindTexture(GL_TEXTURE_2D, Tex);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, 4);
    glBindVertexArray(0);
}
