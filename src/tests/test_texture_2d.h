#pragma once

#include <string>
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
    TestTexture2D(const std::string &InDisplayName);
    ~TestTexture2D();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> m_vao;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture;

    glm::vec3 m_translation_a;
    glm::vec3 m_translation_b;
};

NAMESPACE_END(test)
