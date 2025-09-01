#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "../core.h"
#include "test.h"

class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;

NAMESPACE_BEGIN(test)

class TestBatchRendering : public Test
{
public:
    TestBatchRendering();
    ~TestBatchRendering();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> m_vao;
    std::unique_ptr<VertexBuffer> m_vbo;
    std::unique_ptr<IndexBuffer> m_ibo;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture_0;
    std::unique_ptr<Texture> m_texture_1;

    glm::vec3 m_translation_a;
};

NAMESPACE_END(test)
