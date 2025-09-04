#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
    friend class VertexArray;

    IndexBuffer(const unsigned int *data, unsigned int count, GLenum usage = GL_STATIC_DRAW);
    ~IndexBuffer();

    unsigned int GetCount() const { return m_count; }

private:
    unsigned int m_renderer_id;
    unsigned int m_count;
};
