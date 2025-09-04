#pragma once

#include <GL/glew.h>

/**
 * @brief OpenGL中顶点缓冲的抽象类
 * 举例说明，假设我有一个3D宇宙飞船的模型，我需要绘制这个宇宙飞船
 * 该宇宙飞船会有一个大的VertexBuffer存储它的模型的每个顶点信息
 * 但是它可能会有很多个IndexBuffer，分别负责飞船的不同部分的绘制
 * 因为飞船的窗户和飞船的船身可能使用不同的材质，它们会分布在两次Draw Call。
 */
class VertexBuffer
{
public:
    friend class VertexArray;
    VertexBuffer(const void *data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
    ~VertexBuffer();

    void SubData(const void *data, size_t size);

private:
    unsigned int m_renderer_id;
};
