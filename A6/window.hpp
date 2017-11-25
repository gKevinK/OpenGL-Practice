#ifndef WINDOW_H
#define WINDOW_H

#include <glad\glad.h>
#include <vector>

class Window
{
public:
    unsigned int VAO, VBO;

    Window(float x = -1.0f, float y = -1.0f, float w = 2.0f, float h = 2.0f);
    void Init();
    void Draw(unsigned int shader);

private:
    std::vector<float> vertices;
};


#endif // !WINDOW_H
