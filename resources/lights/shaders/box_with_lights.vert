#version 330 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

out vec3 vNormal;
out vec3 vWorldPosition;
out vec2 vTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.f);
    vWorldPosition = vec3(uModel * vec4(aPosition, 1.f));
    vNormal = aNormal;
    vTexCoords = aTexCoords;
}