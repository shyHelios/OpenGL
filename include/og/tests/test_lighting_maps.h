#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <og/core/core.h>
#include <og/tests/test.h>

class VertexArray;
class Shader;
class Texture;

NAMESPACE_BEGIN(test)

class TestLightingMaps : public Test
{
public:
    TestLightingMaps(const std::string& InDisplayName);
    ~TestLightingMaps();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> BoxVAO;
    std::unique_ptr<VertexArray> LightVAO;
    std::unique_ptr<Shader> BoxShader;
    std::unique_ptr<Shader> LightShader;
    std::unique_ptr<Texture> BoxDiffuseTexture;
    std::unique_ptr<Texture> BoxSpecularTexture;

    glm::vec3 InitialLightPos = glm::vec3(0.f, 20.f, 0.f);
    glm::vec3 LightPos        = glm::vec3(0.f);
    glm::vec3 LightTranslate  = glm::vec3(0.f, -18.5f, 0.f);
    glm::vec3 LightDiffuse    = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 LightSpecular   = glm::vec3(1.f, 1.f, 1.f);

    float BoxScale         = 30.f;
    glm::vec3 BoxTranslate = glm::vec3(-30.f, 0.f, 0.f);

    // 值越大高光范围越小
    float BoxShininess = 25.f;

    float TransformDragSpeed = 0.5f;
    float ColorDragSpeed     = 0.1f;
};

NAMESPACE_END(test)
