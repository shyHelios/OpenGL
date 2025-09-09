#version 330 core

layout(location=0) out vec4 color;

in vec3 vNormal;
in vec3 vWorldPosition;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uCameraPos;

uniform sampler2D uTexture;

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

    // 镜面反射
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(uCameraPos - vWorldPosition);
    vec3 reflectDir = reflect(-lightDir, vNormal); // reflect要求输入的是指向平面的方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    vec3 result = (ambient + diffuse + specular) * uObjectColor;
    color = vec4(vNormal, 1.f);
    // vec3 depthColor = vec3(gl_FragCoord.z);
    // color = vec4(depthColor, 1.f);
}