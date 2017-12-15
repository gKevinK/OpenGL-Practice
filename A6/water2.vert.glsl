#version 430 core
layout(location = 0) in vec2 aPos;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec4 pos = vec4(aPos.x, 0.0, -aPos.y, 1.0);
    gl_Position = pos;
}
