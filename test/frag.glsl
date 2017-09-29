#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 textureCoord;

//uniform vec4 ourColor;
uniform sampler2D texture;

void main()
{
   // FragColor = vec4(1.0f, 0.5f, 1.0f, 1.0f);
   FragColor = texture(texture, textureCoord) * vec4(vertexColor, 1.0);
   // FragColor = ourColor;
}