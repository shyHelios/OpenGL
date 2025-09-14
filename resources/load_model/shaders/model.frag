#version 330 core

out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D uTextureDiffuse0;

void main()
{
    vec4 diffuseColor = texture(uTextureDiffuse0, vTexCoords);
    FragColor = diffuseColor;
}