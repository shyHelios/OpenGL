#include "vertex_buffer.h"

#include <cstring>
#include <iostream>

#include <GL/glew.h>

VertexBuffer::VertexBuffer(const void *data, unsigned int size, GLenum usage)
{
    // 创建缓冲区并将数据上传到GPU
    glCreateBuffers(1, &m_renderer_id);
    // glNamedBufferData(m_renderer_id, size, data, usage);
    // GL_DYNAMIC_STORAGE_BIT保证缓冲可写，GL_MAP_WRITE_BIT保证缓冲可映射且映射后可写
    glNamedBufferStorage(m_renderer_id, size, data, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_renderer_id);
}

void VertexBuffer::SubData(const void * data, size_t size)
{
    // 一般我们需要MAP后可以写就行
    void *ptr = glMapNamedBuffer(m_renderer_id, GL_WRITE_ONLY);
    if (ptr)
    {
        memcpy(ptr, data, size);
        // 传输完必须调用unmap
        glUnmapNamedBuffer(m_renderer_id);
    }
    else
    {
        std::cerr << "Failed to map buffer!" << std::endl;
    }
}
