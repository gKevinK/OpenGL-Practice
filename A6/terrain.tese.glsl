#version 430 core

layout(quads, equal_spacing, cw) in;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform sampler2D heightMap;
uniform sampler2D textureMap;

void main()
{

}