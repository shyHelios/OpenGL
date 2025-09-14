#version 330 core

layout(location=0) out vec4 color;

in vec3 vWorldPosition;
in vec3 vNormal;
in vec2 vTexCoords;

uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uCameraPos;

// 注意glsl中结构体成员变量名和C++中的区别
struct DirectionalLight
{
    vec3 direction;

    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    // 此处光强为1
    float cutOff;
    // 此处光强为0
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;// 影响高光半径
};

uniform Material uMaterial;
uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLights[4];
uniform SpotLight uSpotLight;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// 这个shader没有检测片元是否光源可见，因此会出现看不到光源仍然亮着的情况！
void main()
{
    // 漫反射光照
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(uCameraPos - vWorldPosition);

    // 定向光照
    // vec3 result = CalcDirLight(uDirectionalLight, normal, viewDir);
    vec3 result = vec3(0.f);

    // 点光源
    for (int i = 0; i < 4; i++)
    {
        result += CalcPointLight(uPointLights[i], normal, vWorldPosition, viewDir);
    }

    // 聚光灯
    result += CalcSpotLight(uSpotLight, normal, vWorldPosition, viewDir);

    color = vec4(result, 1.f);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.f);

    // 镜面反射着色
    vec3 reflectDir = reflect(-lightDir, vNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);

    vec3 diffuse = light.diffuse * diff * vec3(texture(uMaterial.diffuse, vTexCoords));
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, vTexCoords));
    return (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(uMaterial.diffuse, vTexCoords));

    // 镜面反射着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, vTexCoords));

    vec3 result = diffuse + specular;
    float distance = length(light.position - fragPos);
    float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    result *= attenuation;
    return result;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(uMaterial.diffuse, vTexCoords));

    // 镜面反射着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, vTexCoords));

    // 计算光照强度
    float cosTheta = dot(lightDir, normalize(-light.direction));
    float deltaCosTheta = light.cutOff - light.outerCutOff;
    float intensity = clamp((cosTheta - light.outerCutOff) / deltaCosTheta, 0.f, 1.f);
    // 聚光灯符合点光源的光强衰减公式
    float distance = length(light.position - fragPos);
    float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 result = diffuse + specular;
    return result * attenuation * intensity;
}
