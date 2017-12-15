#include "water2.hpp"

const float Range = 20.0f;
const float INFTY = 10000.0f;

const float Water2::vertices[]{
   -Range, -Range,
     0.0f, -INFTY,

    Range, -Range,
    INFTY,   0.0f,

    Range,  Range,
     0.0f,  INFTY,

   -Range,  Range,
   -INFTY,   0.0f,

   -Range, -Range,
     0.0f, -INFTY,
};

//const int Water2::indices[]{
//    0, 1, 2,
//    1, 3, 2,
//    2, 3, 4,
//    3, 5, 4,
//    4, 5, 6,
//    5, 7, 6,
//    6, 7, 0,
//    7, 1, 0,
//};

void Water2::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindVertexArray(0);
}

void Water2::Draw(unsigned int shader)
{
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader, "waterTex"), 1);
    glBindTexture(GL_TEXTURE_2D, Tex);
    //glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
    glBindVertexArray(0);
}
