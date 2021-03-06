#version 430 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoord;

//uniform mat4 model;
//uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * vec4(vertex.xy, 0.0, 1.0);
	TexCoord = vertex.zw;
}