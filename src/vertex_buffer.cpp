#include "vertex_buffer.h"

#include <GL/glew.h>

VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
    glGenBuffers(1, &m_renderer_id);
    // 告诉 OpenGL 当前的 GL_ARRAY_BUFFER 就是 vbo
    // 后续针对 GL_ARRAY_BUFFER 的操作（比如 glBufferData、glVertexAttribPointer）都会作用到这个 vbo 上
    glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_renderer_id);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
