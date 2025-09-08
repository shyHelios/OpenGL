#version 330 core

layout(location=0) out vec4 color;

in vec2 v_texCoords;

uniform sampler2D u_Texture;
uniform vec4 u_LightColor;

void main()
{
    color = u_LightColor * texture(u_Texture, v_texCoords);
}