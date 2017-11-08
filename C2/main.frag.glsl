#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

struct Material {
	float diffuse;
	float specular;
	float shininess;
};

struct PointLight {
    vec3 position;
	vec3 color;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

uniform float pointLightNum;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform DirLight dirLight;
uniform PointLight pointLights[10];
uniform SpotLight spotLight;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 BlinnPhone(vec3 normal, vec3 in, vec3 out);

void main()
{
	FragColor = vec4(Color, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	return vec3(1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	return vec3(1.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	return vec3(1.0);
}
