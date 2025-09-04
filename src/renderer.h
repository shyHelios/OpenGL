#pragma once


class VertexArray;
class IndexBuffer;
class Shader;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Clear() const;
    void Draw(const VertexArray &va, const Shader &shader) const;
};
