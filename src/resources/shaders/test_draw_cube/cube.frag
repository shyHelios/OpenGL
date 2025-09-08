#version 330 core

layout(location=0) out vec4 color;

flat in vec3 vNormal;
in vec3 vWorldPosition;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;

void main()
{
    // 环境光照
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * uLightColor;

    // 漫反射光照
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vWorldPosition);
    float diff = max(dot(norm, lightDir), 0);
    vec3 diffuse = diff * uLightColor;

    vec3 result = (ambient + diffuse) * uObjectColor;
    color = vec4(result, 1.f);
}