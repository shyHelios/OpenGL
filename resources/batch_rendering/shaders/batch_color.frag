#version 330 core

layout(location = 0) out vec4 color;

// 索引号，指向某个纹理单元
uniform sampler2D u_Texture[2];
in vec3 v_color;
in vec2 v_tex_coord;
flat in int v_tex_idx;

void main()
{
    vec4 tex_color = texture(u_Texture[v_tex_idx], v_tex_coord);
    color = tex_color;
}