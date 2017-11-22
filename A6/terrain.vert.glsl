#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord_tc;

void main()
{
    gl_Position = vec4(aPos.x, 0.0, -aPos.y, 1.0);
    TexCoord_tc = aTexCoord;
}
