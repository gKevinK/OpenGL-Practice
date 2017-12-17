#include "water.hpp"

const float Range = 20.0f;

//const float Water::vertices[] {
//   -Range, -Range,
//    Range, -Range,
//   -Range,  Range,
//    Range,  Range,
//};



void Water::Init()
{
    int D = 40;
    if (vertices.size() == 0) {
        for (int i = 0; i < D; i++) {
            for (int j = 0; j < D; j++) {
                float fi = (float)i;
                float fj = (float)j;
                float vs[] = {
                    fi, fj,
                    fi + 1, fj,
                    fi, fj + 1,
                    fi + 1, fj + 1
                };
                for (int k = 0; k < 8; k++) {
                    vertices.push_back(vs[k] * Range * 2 / D - Range);
                }
            }
        }
    }
    //float fRange = (float)Range;
    //vertices = {
    //   -fRange, -fRange,
    //    fRange, -fRange,
    //   -fRange,  fRange,
    //    fRange,  fRange,
    //};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindVertexArray(0);
}

void Water::Draw(unsigned int shader)
{
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader, "waterTex"), 1);
    glBindTexture(GL_TEXTURE_2D, Tex);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, vertices.size() / 2);
    glBindVertexArray(0);
}
