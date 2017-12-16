#ifndef WAVE_H
#define WAVE_H

#include <glm/glm.hpp>

struct Wave
{
    float o;
    float s;
    float k;
};

struct WaveDir
{
    glm::vec3 dir;
    float a;
};

#endif // !WAVE_H
