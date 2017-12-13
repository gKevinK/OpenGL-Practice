#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 proj;
uniform mat4 view;
uniform bool invert;

void main()
{
    if (invert) {
        gl_Position = proj * view * vec4(1.0, -1.0, 1.0, 1.0) * vec4(aPos, 1.0);
    } else {
        gl_Position = proj * view * vec4(aPos, 1.0);
    }
    TexCoord = aTexCoord;
}