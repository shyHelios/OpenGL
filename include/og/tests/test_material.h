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

class TestMaterial : public Test
{
public:
    TestMaterial(const std::string& InDisplayName);
    ~TestMaterial();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:
    std::unique_ptr<VertexArray> BoxVAO;
    std::unique_ptr<VertexArray> LightVAO;
    std::unique_ptr<Shader> BoxShader;
    std::unique_ptr<Shader> LightShader;

    glm::vec3 InitialLightPos       = glm::vec3(0.f, 20.f, 0.f);
    glm::vec3 LightPos       = glm::vec3(0.f);
    glm::vec3 LightTranslate = glm::vec3(0.f, -18.5f, 0.f);
    glm::vec3 LightAmbient =glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 LightDiffuse =glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 LightSpecular =glm::vec3(1.f, 1.f, 1.f);

    float BoxScale         = 30.f;
    glm::vec3 BoxTranslate = glm::vec3(-30.f, 0.f, 0.f);

    // cyan plastic材质参数[http://devernay.free.fr/cours/opengl/materials.html]
    glm::vec3 BoxAmbient   = glm::vec3(0.0f, 0.1f, 0.06f);
    glm::vec3 BoxDiffuse   = glm::vec3(0.0f, 0.50980392f, 0.50980392f);
    glm::vec3 BoxSpecular  = glm::vec3(0.50196078f, 0.50196078f, 0.50196078f);
    float BoxShininess     = 25.f; // 值越大高光范围越小

    float TransformDragSpeed = 0.5f;
    float ColorDragSpeed = 0.1f;
};

NAMESPACE_END(test)
