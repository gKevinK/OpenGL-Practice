#include "terrain.hpp"

Terrain::Terrain()
{
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            float fi = (float)i;
            float fj = (float)j;
            float vs[] = {
                fi, fj, fi, fj,
                fi, fj + 1, fi, fj + 1,
                fi + 1, fj, fi + 1, fj,
                fi + 1, fj + 1, fi + 1, fj + 1 };
            for (int k = 0; k < 16; k++) {
                vertices.push_back(vs[k] / 20.0f);
            }
        }
    }
}

void Terrain::Init()
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

void Terrain::Draw(unsigned int shader)
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
