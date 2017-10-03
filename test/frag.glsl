#version 330 core
out vec4 FragColor;

in vec2 textureCoord;

//uniform vec4 ourColor;
uniform sampler2D texture;

void main()
{
   // FragColor = vec4(1.0f, 0.5f, 1.0f, 1.0f);
   FragColor = texture(texture, textureCoord);
   // FragColor = ourColor;
}