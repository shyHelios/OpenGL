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

    glm::vec3 InitialLightPos       = glm::vec3(0.f, 20.f, 0.f);
    glm::vec3 LightPos       = glm::vec3(0.f);
    glm::vec3 LightTranslate = glm::vec3(0.f);
    glm::vec3 LightColor     = glm::vec3(1.f, 1.f, 1.f);

    float BoxScale         = 30.f;
    glm::vec3 BoxTranslate = glm::vec3(-30.f, 0.f, 0.f);
    glm::vec3 BoxColor     = glm::vec3(0.8f, 0.6f, 0.f);

    float DragSpeed = 0.5f;
};

NAMESPACE_END(test)
