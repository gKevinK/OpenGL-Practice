#ifndef SKYBOX2_H
#define SKYBOX2

#include <glad\glad.h>
#include <vector>

class Skybox2
{
public:
    unsigned int VAO, VBO;
    std::vector<unsigned int> TexIds;

    ~Skybox2();
    void Init();
    void Draw(unsigned int shader);

private:
    
    static const float skybox2Vertices[];

};

#endif // !SKYBOX2_H

