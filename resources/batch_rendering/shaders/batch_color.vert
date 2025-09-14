#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in int tex_idx;

uniform mat4 u_MVP;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_color;
out vec2 v_tex_coord; // 使用v_前缀从顶点着色器向片段着色器传递数据，这是一种约定
flat out int v_tex_idx;

void main()
{
    gl_Position = projection * view * position;
    v_tex_coord = tex_coord; // 把纹理坐标传递给片段着色器
    v_tex_idx = tex_idx;
    v_color = color;
}