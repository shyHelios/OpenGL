#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "../core.h"
#include "test.h"

class VertexArray;
class Shader;
class Texture;

NAMESPACE_BEGIN(test)

class TestDrawCube : public Test
{
public:
    TestDrawCube(const std::string& InDisplayName);
    ~TestDrawCube();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> BoxVAO;
    std::unique_ptr<VertexArray> LightVAO;
    std::unique_ptr<Shader> BoxShader;
    std::unique_ptr<Shader> LightShader;
    std::unique_ptr<Texture> m_Texture;

    glm::vec3 LightTranslate = glm::vec3(0.f);
    glm::vec3 BoxTranslate   = glm::vec3(0.f);
};

NAMESPACE_END(test)
