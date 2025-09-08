#pragma once

#include <string>
#include <memory>

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
    std::unique_ptr<VertexArray> VAO;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;
};

NAMESPACE_END(test)
