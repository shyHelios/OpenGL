#pragma once

#include <memory>
#include <string>
#include <array>

#include <glm/glm.hpp>

#include <og/core/core.h>
#include <og/tests/test.h>

class VertexArray;
class Shader;
class Texture;

class PointLight;
class DirectionalLight;
class SpotLight;

NAMESPACE_BEGIN(test)

constexpr int PointLightNum = 4;

class TestLights : public Test
{
public:
    TestLights(const std::string& InDisplayName);
    ~TestLights();

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

    // 光源
    int CurrentPointLightIndex = 0;
    std::array<std::unique_ptr<PointLight>, PointLightNum> m_PointLights;
    std::unique_ptr<DirectionalLight> m_DirectionalLight;
    std::unique_ptr<SpotLight> m_SpotLight;

    bool bDirectionalLightActivated = true;
    bool bPointLightActivated       = false;
    bool bSpotLightActivated        = false;

    glm::vec3 LightTranslate = glm::vec3(0.f, 0.f, -7.f);

    float BoxScale = 1.5f;

    // 值越大高光范围越小
    float BoxShininess = 1.5f;

    float TransformDragSpeed = 0.5f;
    float ColorDragSpeed     = 0.1f;
};

NAMESPACE_END(test)
