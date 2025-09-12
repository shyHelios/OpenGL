#include "test_lights.h"

#include <format>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <imgui.h>

#include "../camera.h"
#include "../index_buffer.h"
#include "../renderer.h"
#include "../shader.h"
#include "../texture.h"
#include "../vertex_array.h"
#include "../vertex_buffer.h"
#include "../vertex_buffer_layout.h"

extern Camera MyCamera;

struct DirectionalLight
{
    // 从光源向外发射的方向
    glm::vec3 Direction;

    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
};

struct PointLight
{
    glm::vec3 Position;

    glm::vec3 Diffuse;
    glm::vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;

    glm::vec3 LightTranslate_ = glm::vec3(0.f, 0.f, -7.f);
};

struct SpotLight
{
    glm::vec3 Position;
    glm::vec3 Direction;

    // 内圆锥的半角余弦值
    float CutOff;
    // 外圆锥的半角余弦值
    float OuterCutOff;

    // 用户调节用的度数
    float CutOffAngle_;
    float OuterCutOffAngle_;

    glm::vec3 Diffuse;
    glm::vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

NAMESPACE_BEGIN(test)

TestLights::TestLights(const std::string& InDisplayName) :
    Test(InDisplayName)
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置相机pose
    MyCamera.SetPosition(glm::vec3(12.3227f, 3.40986f, -8.0141f));
    MyCamera.SetPitch(-12.7f);
    MyCamera.SetYaw(-190.7f);

    // 设置光源
    for (int i = 0; i < PointLightNum; i++)
    {
        std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>();
        pointLight->Constant                   = 1.f;
        pointLight->Linear                     = 0.09f;
        pointLight->Quadratic                  = 0.032f;
        pointLight->Diffuse                    = glm::vec3(1.f);
        pointLight->Specular                   = glm::vec3(1.f);
        m_PointLights[i]                       = std::move(pointLight);
    }

    m_DirectionalLight           = std::make_unique<DirectionalLight>();
    m_DirectionalLight->Diffuse  = glm::vec3(1.f);
    m_DirectionalLight->Specular = glm::vec3(1.f);

    m_SpotLight                    = std::make_unique<SpotLight>();
    m_SpotLight->Diffuse           = glm::vec3(1.f);
    m_SpotLight->Specular          = glm::vec3(1.f);
    m_SpotLight->Constant          = 1.f;
    m_SpotLight->Linear            = 0.09f;
    m_SpotLight->Quadratic         = 0.032f;
    m_SpotLight->CutOffAngle_      = 12.5f;
    m_SpotLight->OuterCutOffAngle_ = 30.f;

    // 物体vao
    BoxVAO = std::make_unique<VertexArray>();

    float vertices[] = {
            // positions(3 float) normal(3 float) texture coords(2 float)
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

            -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

            -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};
    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    VertexBufferLayout boxLayout;
    boxLayout.PushAttribute<float>(3);
    boxLayout.PushAttribute<float>(3);
    boxLayout.PushAttribute<float>(2);
    BoxVAO->AddBuffer(vertexBuffer, boxLayout);

    // 光源vao
    LightVAO = std::make_unique<VertexArray>();
    VertexBufferLayout lightLayout;
    lightLayout.PushAttribute<float>(3);
    lightLayout.PushAttribute<float>(3);
    lightLayout.PushAttribute<float>(2);
    LightVAO->AddBuffer(vertexBuffer, lightLayout);

    std::unordered_map<unsigned int, std::string> boxShaderFiles{
            {GL_VERTEX_SHADER,   "resources/lights/shaders/box_with_lights.vert"},
            {GL_FRAGMENT_SHADER, "resources/lights/shaders/box_with_lights.frag"}
    };
    BoxShader = std::make_unique<Shader>(boxShaderFiles);

    std::unordered_map<unsigned int, std::string> lightShaderFiles{
            {GL_VERTEX_SHADER,   "resources/lights/shaders/light.vert"},
            {GL_FRAGMENT_SHADER, "resources/lights/shaders/light.frag"}
    };
    LightShader = std::make_unique<Shader>(lightShaderFiles);

    BoxDiffuseTexture  = std::make_unique<Texture>("resources/lights/textures/container2.png", 0);
    BoxSpecularTexture = std::make_unique<Texture>("resources/lights/textures/container2_specular.png", 1);
}

TestLights::~TestLights()
{
    glDisable(GL_DEPTH_TEST);
}

void TestLights::OnUpdate(float delta_time)
{
    // 根据用户输入更新PointLights位置
    for (int i = 0; i < PointLightNum; i++)
    {
        m_PointLights[i]->Position = m_PointLights[i]->LightTranslate_;
    }

    m_SpotLight->CutOff      = std::cosf(m_SpotLight->CutOffAngle_);
    m_SpotLight->OuterCutOff = std::cosf(m_SpotLight->OuterCutOffAngle_);
}

void TestLights::OnRender()
{
    glm::mat4 projection = MyCamera.GetProjectionMatrix();
    glm::mat4 view       = MyCamera.GetViewMatrix();

    BoxShader->Bind();
    BoxShader->SetUniformMat4f("uView", view);
    BoxShader->SetUniformMat4f("uProjection", projection);
    BoxShader->SetUniform3f("uCameraPos", MyCamera.GetPosition());

    BoxShader->SetUniform1i("uMaterial.diffuse", 0);
    BoxShader->SetUniform1i("uMaterial.specular", 1);
    BoxShader->SetUniform1f("uMaterial.shininess", BoxShininess);

    // 方向光源配置
    m_DirectionalLight->Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    BoxShader->SetUniform3f("uDirectionalLight.direction", m_DirectionalLight->Direction);
    BoxShader->SetUniform3f("uDirectionalLight.diffuse", m_DirectionalLight->Diffuse);
    BoxShader->SetUniform3f("uDirectionalLight.specular", m_DirectionalLight->Specular);

    // 点光源配置
    glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
                                       glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};
    for (int i = 0; i < PointLightNum; i++)
    {
        BoxShader->SetUniform3f(std::format("uPointLights[{}].position", i), pointLightPositions[i]);
        BoxShader->SetUniform3f(std::format("uPointLights[{}].diffuse", i), m_PointLights[i]->Diffuse);
        BoxShader->SetUniform3f(std::format("uPointLights[{}].specular", i), m_PointLights[i]->Specular);

        BoxShader->SetUniform1f(std::format("uPointLights[{}].constant", i), m_PointLights[i]->Constant);
        BoxShader->SetUniform1f(std::format("uPointLights[{}].linear", i), m_PointLights[i]->Linear);
        BoxShader->SetUniform1f(std::format("uPointLights[{}].quadratic", i), m_PointLights[i]->Quadratic);
    }

    // 聚光灯配置
    BoxShader->SetUniform3f("uSpotLight.position", MyCamera.GetPosition());
    BoxShader->SetUniform3f("uSpotLight.direction", MyCamera.GetFront());
    BoxShader->SetUniform3f("uSpotLight.diffuse", m_SpotLight->Diffuse);
    BoxShader->SetUniform3f("uSpotLight.specular", m_SpotLight->Specular);
    BoxShader->SetUniform1f("uSpotLight.cutOff", m_SpotLight->CutOff);
    BoxShader->SetUniform1f("uSpotLight.outerCutOff", m_SpotLight->OuterCutOff);
    BoxShader->SetUniform1f("uSpotLight.constant", m_SpotLight->Constant);
    BoxShader->SetUniform1f("uSpotLight.linear", m_SpotLight->Linear);
    BoxShader->SetUniform1f("uSpotLight.quadratic", m_SpotLight->Quadratic);

    Renderer renderer;

    // 10个箱子的位置
    glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                 glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                 glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
    // 绘制10个箱子，分10次绘制，没有考虑到箱子之间的遮挡作用，以及光线在箱子之间的弹射
    // 单个箱子的自遮挡也没有考虑
    for (int i = 0; i < 10; i++)
    {
        glm::mat4 modelBox = glm::mat4(1.f);
        // 依次应用translate rotate scale结果为model*T*R*S，因此最后添加的变换先应用
        modelBox = glm::translate(modelBox, cubePositions[i]);
        modelBox = glm::rotate(modelBox, glm::radians(20.f * i), glm::vec3(0.f, 1.f, 0.f));
        modelBox = glm::scale(modelBox, glm::vec3(BoxScale));
        BoxShader->SetUniformMat4f("uModel", modelBox);
        renderer.Draw(*BoxVAO, *BoxShader);
    }

    // 绘制点光源立方体，便于确认光源位置
    LightShader->Bind();
    LightShader->SetUniformMat4f("uView", view);
    LightShader->SetUniformMat4f("uProjection", projection);
    LightShader->SetUniform3f("uLightColor", glm::vec3(1.f, 1.f, 1.f));

    for (int i = 0; i < PointLightNum; i++)
    {
        glm::mat4 modelLight = glm::translate(glm::mat4(1.f), m_PointLights[i]->LightTranslate_);
        LightShader->SetUniformMat4f("uModel", modelLight);
        renderer.Draw(*LightVAO, *LightShader);
    }
}

void TestLights::OnImGuiRender()
{
    // 点光源设置
    ImGui::Text("点光源设置");
    ImGui::Separator();
    std::array<std::string, PointLightNum> items;
    const char* cItems[PointLightNum];
    for (int i = 0; i < PointLightNum; i++)
    {
        items[i]  = std::to_string(i);
        cItems[i] = items[i].c_str();
    }
    ImGui::Combo("当前点光源序号", &CurrentPointLightIndex, cItems, PointLightNum);
    ImGui::DragFloat3("点光源平移量", &m_PointLights[CurrentPointLightIndex]->LightTranslate_[0], TransformDragSpeed,
                      -100.f, 100.f, "%.2f");
    ImGui::ColorEdit3("点光源漫反射颜色", &m_PointLights[CurrentPointLightIndex]->Diffuse[0],
                      ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("点光源镜面反射颜色", &m_PointLights[CurrentPointLightIndex]->Specular[0],
                      ImGuiColorEditFlags_Float);

    // 方向光源设置
    // FIXME: 补充方向光源角度设置控件
    ImGui::Text("方向光源设置");
    ImGui::Separator();
    ImGui::ColorEdit3("方向光源漫反射颜色", &m_DirectionalLight->Diffuse[0], ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("方向光源镜面反射颜色", &m_DirectionalLight->Specular[0], ImGuiColorEditFlags_Float);

    // 聚光灯设置聚光灯的位置和方向默认不可调，和我们摄像机的位置和朝向一致
    ImGui::Text("手电筒设置");
    ImGui::Separator();
    ImGui::DragFloat("内圆锥半角", &m_SpotLight->CutOffAngle_, TransformDragSpeed, 0.f, m_SpotLight->OuterCutOffAngle_,
                     "%.2f");
    ImGui::DragFloat("外圆锥半角", &m_SpotLight->OuterCutOffAngle_, TransformDragSpeed, m_SpotLight->CutOffAngle_, 90.f,
                     "%.2f");
    ImGui::ColorEdit3("手电筒漫反射颜色", &m_SpotLight->Diffuse[0], ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("手电筒镜面反射颜色", &m_SpotLight->Specular[0], ImGuiColorEditFlags_Float);

    // 立方体设置
    ImGui::Text("立方体设置");
    ImGui::Separator();
    ImGui::DragFloat("立方体缩放", &BoxScale, TransformDragSpeed, 0.001f, 100.f, "%.2f");
    // 材质
    ImGui::DragFloat("高光度", &BoxShininess, TransformDragSpeed, 1.f, 64.f, "%.1f");
}


NAMESPACE_END(test)
