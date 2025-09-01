#include "texture.h"

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <stb_image.h>

Texture::Texture(const std::string &filepath, unsigned int slot) :
    m_filepath(filepath)
{
    // png读取从左上到右下，OpenGL从左下到右上，需要flip才对
    stbi_set_flip_vertically_on_load(1);

    // desired channels设置为0维持原图channels不变
    m_local_buffer = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channels, 0);

    glGenTextures(1, &m_renderer_id);
    // 执行Bind前必须先激活槽位，否则新的Texture创建可能覆盖上一个Texture槽位
    // OpenGL4.5的API，替代glActiveTexture(GL_TEXTURE0 + slot)
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
    // glBindTextureUnit(slot, m_renderer_id);

    glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置texture缠绕方式，
    // GL_CLAMP_TO_EDGE，超出边界的clamp到边界，坐标<0，使用0，坐标大于1，使用1，纹理会被拉成条带
    // GL_REPEAT，1.1取0.1，就是平铺
    // GL_MIRRORED_REPEAT，1.1取0.9，-0.1取0.1
    // GL_CLAMP_TO_BORDER，使用用户设置的border颜色(glTexParameterfv)，如果没有设置，使用黑色，范围外是大片border
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 把纹理传递给GPU，这里的internal format参数指定OpenGL将以什么格式存储我们的数据
    // 尽管可以使用GL_RGBA，但是后面跟数字指定每个channel的存储位数是更优的方式，因此使用GL_RGBA8
    // level参数在我们手动上传多张纹理组成mipmap时使用，可以指定每个纹理属于哪一层，但是一般不用，一般调用glGenerateMipmap直接生成mipmap
    // glTextureImage2DEXT(m_renderer_id, GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    //                     m_local_buffer);

    // 分配存储（现代 DSA，替代 glTexImage2D）
    glTextureStorage2D(m_renderer_id, 1, GL_RGBA8, m_width, m_height);
    // 上传像素数据
    glTextureSubImage2D(m_renderer_id, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_local_buffer);

    // 释放内存中的图片
    if (m_local_buffer)
    {
        stbi_image_free(m_local_buffer);
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_renderer_id);
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
}
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
