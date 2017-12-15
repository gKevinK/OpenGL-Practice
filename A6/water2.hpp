#ifndef WATER_H
#define WATER_H

#include <glad\glad.h>

class Water2
{
public:
    unsigned int VAO, VBO, EBO;
    unsigned int Tex;

    //Water();
    void Init();
    //void Update(float x, float y, float z);
    void Draw(unsigned int shader);

private:
    const static float vertices[16];
    const static int indices[24];
};

#endif // !WATER_H
