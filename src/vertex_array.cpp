#include "vertex_array.h"

#include <vector>

#include <GL/glew.h>

#include "index_buffer.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &m_renderer_id);
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

void VertexArray::AddBuffer(const VertexBuffer &vertex_buffer, const VertexBufferLayout &layout)
{
    // 第二个参数是binding point，第四个参数是第一个vertex的偏移量，一般都是0
    glVertexArrayVertexBuffer(m_renderer_id, 0, vertex_buffer.m_renderer_id, 0, layout.GetStride());
    const std::vector<VertexAttribute> attributes = layout.GetAttributes();

    // offset指定每个属性在顶点中的偏移量
    unsigned int offset = 0;
    // 每个顶点有多少个属性就要调用多少次
    for (unsigned int i = 0; i < attributes.size(); i++)
    {
        const VertexAttribute &attribute = attributes[i];

        glEnableVertexArrayAttrib(m_renderer_id, i);
        // 定义attribute数据格式
        glVertexArrayAttribFormat(m_renderer_id, i, attribute.count, attribute.type, attribute.normalized, offset);
        // 把attribute映射到某个binding point
        glVertexArrayAttribBinding(m_renderer_id, i, 0);

        offset += attribute.count * attribute.GetSizeOfType(attribute.type);
    }
}

void VertexArray::AddBuffer(const IndexBuffer &index_buffer)
{
    glVertexArrayElementBuffer(m_renderer_id, index_buffer.m_renderer_id);
}
