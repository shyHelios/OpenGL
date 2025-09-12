#version 330 core

layout(location=0) out vec4 color;

in vec3 vWorldPosition;
in vec3 vNormal;
in vec2 vTexCoords;

uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uCameraPos;

// 这个点光源不会衰减的
struct Light
{
    vec3 position;
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
uniform Light uLight;

// 这个shader没有检测片元是否光源可见，因此会出现看不到光源仍然亮着的情况！
void main()
{
    // 漫反射光照
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLight.position - vWorldPosition);
    float diff = max(dot(norm, lightDir), 0);
    vec3 diffuse = uLight.diffuse * (diff * vec3(texture(uMaterial.diffuse, vTexCoords)));

    // 镜面反射
    vec3 viewDir = normalize(uCameraPos - vWorldPosition);
    vec3 reflectDir = reflect(-lightDir, vNormal); // reflect要求输入的是指向平面的方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);
    vec3 specular = uLight.specular * (spec * vec3(texture(uMaterial.specular, vTexCoords)));

    vec3 result = diffuse + specular;
    color = vec4(result, 1.f);
}