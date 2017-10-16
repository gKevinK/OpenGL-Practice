#version 430 core
out vec4 FragColor;

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

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform float pointLightNum;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform DirLight dirLight;
uniform PointLight pointLights[10];
uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D texture1;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = ambient * material.diffuse;
	result += CalcDirLight(dirLight, norm, viewDir);
	for (int i = 0; i < pointLightNum; i++) {
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	FragColor = vec4(result, 1.0);
    FragColor *= texture(texture1, TexCoord);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 diffuse = diff * vec3(material.diffuse);
	vec3 specular = spec * vec3(material.specular);
	return (diffuse + specular) * light.color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
	float distance = length(light.position - fragPos);
	vec3 diffuse = diff * vec3(material.diffuse);
	vec3 specular = spec * vec3(material.specular);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
		light.quadratic * (distance * distance));
	diffuse *= attenuation;
	specular *= attenuation;
	return (diffuse + specular) * light.color;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	vec3 diffuse = diff * vec3(material.diffuse);
	vec3 specular = spec * vec3(material.specular);
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (diffuse + specular) * light.color;
}
