#version 430 core
layout(location = 0) in vec2 aPos;

uniform vec3 viewPos;

void main()
{
    gl_Position = vec4(aPos.x + viewPos.x, 0.0, -aPos.y + viewPos.z, 1.0);
}
