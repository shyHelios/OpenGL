#version 330 core

layout(location=0) out vec4 color;

in vec3 vNormal;
in vec3 vWorldPosition;

uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uCameraPos;

uniform sampler2D uTexture;

// 注意glsl中结构体成员变量名和C++中的区别
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;// 影响高光半径
};

uniform Material uMaterial;
uniform Light uLight;

// 这个shader没有检测片元是否光源可见，因此会出现看不到光源仍然亮着的情况！
void main()
{
    // 环境光照
    vec3 ambient = uLight.ambient * uMaterial.ambient * vec3(0.1f);

    // 漫反射光照
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLight.position - vWorldPosition);
    float diff = max(dot(norm, lightDir), 0);
    vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

    // 镜面反射
    vec3 viewDir = normalize(uCameraPos - vWorldPosition);
    vec3 reflectDir = reflect(-lightDir, vNormal); // reflect要求输入的是指向平面的方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);
    vec3 specular = uLight.specular * (spec * uMaterial.specular);

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.f);
}