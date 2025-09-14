#pragma once

#include <memory>
#include <string>

#include <og/core/core.h>
#include <og/tests/test.h>

class Shader;
class Model;

NAMESPACE_BEGIN(test)

class TestLoadModel : public Test
{
public:
    TestLoadModel(const std::string& InDisplayName);
    ~TestLoadModel();

    void OnRender() override;
    void OnImGuiRender() override;

private:
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Model> m_Model;
};

NAMESPACE_END(test)
