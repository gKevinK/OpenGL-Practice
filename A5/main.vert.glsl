#version 430 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoord;

uniform float x;
uniform float y;
uniform float w;
uniform float h;

void main()
{
	float xpos = mix(x, x + w, aPos.x);
	float ypos = mix(y, y + h, aPos.y);
    gl_Position = vec4(xpos, ypos, -1.0, 1.0); 
    TexCoord = aPos;
}