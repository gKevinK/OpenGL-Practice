#ifndef WINDOW_H
#define WINDOW_H

#include <glad\glad.h>
#include <vector>

class Window
{
public:
    unsigned int VAO, VBO;
    float x, y, w, h;

    Window(float wx = -1.0f, float wy = -1.0f, float ww = 2.0f, float wh = 2.0f);
    void Init();
    void Draw(unsigned int shader);

private:
    const static float vertices[12];
};


#endif // !WINDOW_H
