#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

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
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

//struct SpotLight {
//    vec3 position;
//    vec3 direction;
//    vec3 color;
//    float constant;
//    float linear;
//    float quadratic;
//    float cutOff;
//    float outerCutOff;
//};

uniform int pointLightNum;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform DirLight dirLight;
uniform PointLight pointLights[8];
//uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D tex;
const float gamma = 2.2;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float BlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = ambient * material.diffuse;
    result += CalcDirLight(dirLight, norm, viewDir);
    for (int i = 0; i < pointLightNum; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    FragColor = vec4(result * vec3(texture(tex, TexCoord)), 1.0);
    FragColor.rgb = pow(FragColor.rgb, 1 / vec3(gamma));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = BlinnPhong(normal, lightDir, viewDir, material.shininess);
    return light.color * (diff * material.diffuse + spec * material.specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = BlinnPhong(normal, lightDir, viewDir, material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
        light.quadratic * (distance * distance));
    return light.color * (diff * material.diffuse + spec * material.specular) * attenuation;
}

//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    float diff = max(dot(normal, lightDir), 0.0);
//    float spec = BlinnPhong(normal, lightDir, viewDir, material.shininess);
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance +
//        light.quadratic * (distance * distance));
//    float theta = dot(lightDir, normalize(-light.direction));
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//    return light.color * (diff * material.diffuse + spec * material.specular) * attenuation * intensity;
//}

float BlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess)
{
    vec3 halfway = normalize(lightDir + viewDir);
    return pow(max(dot(halfway, normal), 0.0), shininess);
}
