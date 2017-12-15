#ifndef WATER_H
#define WATER_H

#include <glad\glad.h>

class Water
{
public:
    unsigned int VAO, VBO;
    unsigned int Tex;

    //Water();
    void Init();
    //void Update(float x, float y, float z);
    void Draw(unsigned int shader);

private:
    const static float vertices[8];
};

#endif // !WATER_H
