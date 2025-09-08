#include "fbo.h"

#include <iostream>

#include <GL/glew.h>

FrameBuffer::FrameBuffer(int InWidth, int InHeight) :
    Width(InWidth),
    Height(InHeight)
{
    glCreateFramebuffers(1, &RenderID);

    // 创建颜色纹理附件
    glCreateTextures(GL_TEXTURE_2D, 1, &TexColorBufferID);
    glTextureParameteri(TexColorBufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(TexColorBufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 对于frame buffer一般用不到缠绕方式
    // glTextureParameteri(TexColorBufferID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTextureParameteri(TexColorBufferID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureStorage2D(TexColorBufferID, 1, GL_RGBA8, Width, Height); // 分配存储空间，但是不上传数据
    // glTextureSubImage2D(TexColorBufferID, 0, 0, 0, WindowWidth * 2 / 3, WindowHeight, GL_RGBA, GL_UNSIGNED_BYTE,
    //                     m_local_buffer);

    glNamedFramebufferTexture(RenderID, GL_COLOR_ATTACHMENT0, TexColorBufferID, 0);

    if (glCheckNamedFramebufferStatus(RenderID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FBO 创建失败!" << std::endl;
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &RenderID);
}

void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, RenderID);
}

void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::GetTexColorBufferID() const
{
    return TexColorBufferID;
}

int FrameBuffer::GetWidth() const
{
    return Width;
}

int FrameBuffer::GetHeight() const
{
    return Height;
}
