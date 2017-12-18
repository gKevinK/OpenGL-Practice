#version 430 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

void main()
{
	vec3 diffuse = texture(texture_diffuse1, TexCoords);
	vec3 specular = texture(texture_specular1, TexCoords);
	vec3 normal = texture(texture_normal1, TexCoords);
	FragColor = texture(texture_diffuse1, TexCoords);
}