#include "vertex_buffer.h"

#include <GL/glew.h>

VertexBuffer::VertexBuffer(const void *data, unsigned int size, GLenum usage)
{
    // 创建缓冲区并将数据上传到GPU
    glCreateBuffers(1, &m_renderer_id);
    glNamedBufferData(m_renderer_id, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_renderer_id);
}
