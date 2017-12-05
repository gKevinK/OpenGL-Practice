#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad\glad.h>
#include <vector>

class Terrain
{
public:
    unsigned int VAO, VBO;
    unsigned int TextureMap, HeightMap, DetailMap;

    Terrain();
    void Init();
    void Draw(unsigned int shader);

private:
    std::vector<float> vertices;
};
#endif // !TERRAIN_H

