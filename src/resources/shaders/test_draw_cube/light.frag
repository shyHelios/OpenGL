#version 330 core

out vec4 color;
uniform vec3 uLightColor;

void main()
{
    color = vec4(uLightColor, 1.f);
}