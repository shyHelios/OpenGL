#include "vertex_array.h"

#include <vector>

#include <GL/glew.h>

#include "vertex_buffer_layout.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_renderer_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_renderer_id);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_renderer_id);
}
void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddBuffer(const VertexBuffer &buffer, const VertexBufferLayout &layout)
{
    const std::vector<VertexAttribute> attributes = layout.GetAttributes();

    // offset指定每个属性在顶点中的偏移量
    unsigned int offset = 0;
    for (unsigned int i = 0; i < attributes.size(); i++)
    {
        const VertexAttribute &attribute = attributes[i];
        // 每个顶点有多少个属性就要调用多少次
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalized, layout.GetStride(),
                              (const void *) offset);
        offset += attribute.count * attribute.GetSizeOfType(attribute.type);
    }
}
