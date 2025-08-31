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

class TestTexture2D : public Test
{
public:
    TestTexture2D();
    ~TestTexture2D();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> m_vao;
    std::unique_ptr<VertexBuffer> m_vbo;
    std::unique_ptr<IndexBuffer> m_ibo;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture;

    glm::vec3 m_translation_a;
    glm::vec3 m_translation_b;
};

NAMESPACE_END(test)
