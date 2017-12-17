#ifndef WATER_H
#define WATER_H

#include <glad\glad.h>
#include <vector>

class Water
{
public:
    unsigned int VAO, VBO;
    unsigned int Tex;

    //Water();
    void Init();
    void Draw(unsigned int shader);

private:
    std::vector<float> vertices;
};

#endif // !WATER_H
