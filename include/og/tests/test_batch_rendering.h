#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include <og/core/core.h>
#include <og/tests/test.h>

class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;

NAMESPACE_BEGIN(test)

class TestBatchRendering : public Test
{
public:
    TestBatchRendering(const std::string &InDisplayName);
    ~TestBatchRendering();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> m_vao;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture_0;
    std::unique_ptr<Texture> m_texture_1;

    glm::vec3 m_translation_a;
};

NAMESPACE_END(test)
