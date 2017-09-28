#version 330 core
in vec3 vertexColor;
out vec4 FragColor;
//uniform vec4 ourColor;

void main()
{
   // FragColor = vec4(1.0f, 0.5f, 1.0f, 1.0f);
   FragColor = vec4(vertexColor, 1.0f);
   // FragColor = ourColor;
}