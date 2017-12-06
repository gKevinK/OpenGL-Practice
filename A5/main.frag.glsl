#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{ 
    // FragColor = texture(tex, TexCoord);
	FragColor = vec4(0.2, 0.2, 0.2, 1.0);
}