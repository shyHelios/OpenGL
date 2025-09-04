#include "index_buffer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count, GLenum usage) :
    m_count(count)
{
    // glGenBuffers(1, &m_renderer_id);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
    glCreateBuffers(1, &m_renderer_id);
    glNamedBufferData(m_renderer_id, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {}
