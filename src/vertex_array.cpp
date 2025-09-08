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

void VertexArray::AddBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer, const VertexBufferLayout& layout)
{
    // 存储指针防止VertexBuffer析构
    m_vertex_buffers.emplace_back(vertex_buffer);
    // 第二个参数是binding point，第四个参数是第一个vertex的偏移量，一般都是0
    glVertexArrayVertexBuffer(m_renderer_id, 0, vertex_buffer->m_renderer_id, 0, layout.GetStride());
    const std::vector<VertexAttribute> attributes = layout.GetAttributes();

    // 更新顶点数
    VertexCount += vertex_buffer->GetSize() / layout.GetStride();

    // offset指定每个属性在顶点中的偏移量
    unsigned int offset = 0;
    // 每个顶点有多少个属性就要调用多少次
    for (unsigned int i = 0; i < attributes.size(); i++)
    {
        const VertexAttribute& attribute = attributes[i];

        glEnableVertexArrayAttrib(m_renderer_id, i);
        // 定义attribute数据格式
        glVertexArrayAttribFormat(m_renderer_id, i, attribute.count, attribute.type, attribute.normalized, offset);
        // 把attribute i映射到binding point 0
        glVertexArrayAttribBinding(m_renderer_id, i, 0);

        offset += attribute.count * attribute.GetSizeOfType(attribute.type);
    }
}

void VertexArray::AddBuffer(const std::shared_ptr<IndexBuffer>& index_buffer)
{
    // 存储指针防止IndexBuffer析构
    m_index_buffer = index_buffer;
    glVertexArrayElementBuffer(m_renderer_id, index_buffer->m_renderer_id);
}

unsigned int VertexArray::GetIndexCount() const
{
    return m_index_buffer->GetCount();
}

unsigned int VertexArray::GetVertexCount() const
{
    return VertexCount;
}

bool VertexArray::HasIndexBuffer() const
{
    return m_index_buffer.get() != nullptr;
}
