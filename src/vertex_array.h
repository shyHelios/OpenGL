#pragma once

#include <memory>
#include <vector>

class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddBuffer(const std::shared_ptr<VertexBuffer> &vertex_buffer, const VertexBufferLayout &layout);
    void AddBuffer(const std::shared_ptr<IndexBuffer> &index_buffer);

    unsigned int GetVertexCount() const;
    bool HasIndexBuffer() const;

    /**
     * @brief 获取索引数量
     * 
     * @return unsigned int 索引数量
     */
    unsigned int GetIndexCount() const;

private:
    unsigned int m_renderer_id;

    std::vector<std::shared_ptr<VertexBuffer>> m_vertex_buffers;
    std::shared_ptr<IndexBuffer> m_index_buffer;

    unsigned int VertexCount = 0;
};
