#pragma once

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

    void AddBuffer(const VertexBuffer &vertex_buffer, const VertexBufferLayout &layout);
    void AddBuffer(const IndexBuffer &index_buffer);

private:
    unsigned int m_renderer_id;
};
