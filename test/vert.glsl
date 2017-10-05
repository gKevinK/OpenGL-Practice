#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 transform;

out vec2 textureCoord;

void main()
{
   gl_Position = proj * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   textureCoord = aTexCoord;
}
