#version 330 core

layout(location = 0) out vec4 color;

// 索引号，指向某个纹理单元
uniform sampler2D u_Texture[2];
in vec3 v_color;
in vec2 v_tex_coord;
in float v_tex_idx;

void main()
{
    int tex_idx = int(v_tex_idx);
    vec4 tex_color = texture(u_Texture[tex_idx], v_tex_coord);
    color = tex_color;
}