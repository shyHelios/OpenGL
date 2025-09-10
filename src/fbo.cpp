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
    // frame buffer的纹理附件用不到缠绕方式
    glTextureStorage2D(TexColorBufferID, 1, GL_RGBA8, Width, Height); // 分配存储空间，但是不上传数据
    glNamedFramebufferTexture(RenderID, GL_COLOR_ATTACHMENT0, TexColorBufferID, 0);

    // 创建深度纹理附件
    glCreateTextures(GL_TEXTURE_2D, 1, &TexDepthBufferID);
    glTextureParameteri(TexDepthBufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(TexDepthBufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 格式有很多种，GL_DEPTH_COMPONENT32(F)/GL_DEPTH_COMPONENT24/GL_DEPTH_COMPONENT16
    glTextureStorage2D(TexDepthBufferID, 1, GL_DEPTH_COMPONENT32, Width, Height); // 分配存储空间，但是不上传数据
    glNamedFramebufferTexture(RenderID, GL_DEPTH_ATTACHMENT, TexDepthBufferID, 0);

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
    // GL_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER + GL_READ_FRAMEBUFFER
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
