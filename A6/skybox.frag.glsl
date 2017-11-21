#version 430 core
in vec3 TexCoord;
out vec4 Color;

uniform samplerCube skybox;

void main()
{
    Color = texture(skybox, TexCoord);
}