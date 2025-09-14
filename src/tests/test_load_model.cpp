#include <og/tests/test_load_model.h>

#include <memory>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <og/og.h>

extern Camera MyCamera;

NAMESPACE_BEGIN(test)

TestLoadModel::TestLoadModel(const std::string& InDisplayName) :
    Test(InDisplayName)
{
    MyCamera.SetPosition(glm::vec3(0.f, 0.f, 12.4861f));

    m_Model = std::make_unique<Model>("resources/load_model/model/backpack/backpack.obj");

    std::unordered_map<unsigned int, std::string> shaderFiles{
            {GL_VERTEX_SHADER,   "resources/load_model/shaders/model.vert"},
            {GL_FRAGMENT_SHADER, "resources/load_model/shaders/model.frag"}
    };
    m_Shader = std::make_unique<Shader>(shaderFiles);
}

TestLoadModel::~TestLoadModel() {}

void TestLoadModel::OnRender()
{
    // 设置uniform参数
    glm::mat4 modelMatrix      = glm::mat4(1.f);
    glm::mat4 viewMatrix       = MyCamera.GetViewMatrix();
    glm::mat4 projectionMatrix = MyCamera.GetProjectionMatrix();
    
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("uModel", modelMatrix);
    m_Shader->SetUniformMat4f("uView", viewMatrix);
    m_Shader->SetUniformMat4f("uProjection", projectionMatrix);

    // 纹理采样器由模型内部的mesh自动设置
    m_Model->Draw(*m_Shader);
}

void TestLoadModel::OnImGuiRender() {}

NAMESPACE_END(test)
