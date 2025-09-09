#include "test_draw_cube.h"

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

NAMESPACE_BEGIN(test)

TestDrawCube::TestDrawCube(const std::string& InDisplayName) :
    Test(InDisplayName)
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 物体vao
    BoxVAO = std::make_unique<VertexArray>();

    float vertices[] = {// 后面
                        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f,
                        -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
                        -1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                        // 前面
                        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f,
                        0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                        // 左面
                        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f,
                        -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, -1.0f,
                        0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
                        // 右面
                        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f,
                        1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
                        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
                        // 下面
                        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f,
                        0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, -1.0f,
                        0.0f, -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
                        // 上面
                        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
                        0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    VertexBufferLayout boxLayout;
    boxLayout.PushAttribute<float>(3);
    boxLayout.PushAttribute<float>(3);
    BoxVAO->AddBuffer(vertexBuffer, boxLayout);

    // 光源vao
    LightVAO = std::make_unique<VertexArray>();
    VertexBufferLayout lightLayout;
    lightLayout.PushAttribute<float>(3);
    lightLayout.PushAttribute<float>(3);
    LightVAO->AddBuffer(vertexBuffer, lightLayout);

    std::unordered_map<unsigned int, std::string> boxShaderFiles{
            {GL_VERTEX_SHADER,   "resources/shaders/test_draw_cube/cube.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/test_draw_cube/cube.frag"}
    };
    BoxShader = std::make_unique<Shader>(boxShaderFiles);

    std::unordered_map<unsigned int, std::string> lightShaderFiles{
            {GL_VERTEX_SHADER,   "resources/shaders/test_draw_cube/light.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/test_draw_cube/light.frag"}
    };
    LightShader = std::make_unique<Shader>(lightShaderFiles);

    m_Texture = std::make_unique<Texture>("resources/dog.png");
}

TestDrawCube::~TestDrawCube()
{
    glDisable(GL_DEPTH_TEST);
}

void TestDrawCube::OnUpdate(float delta_time)
{
    LightPos = InitialLightPos + LightTranslate;
}

void TestDrawCube::OnRender()
{
    glm::mat4 projection = MyCamera.GetProjectionMatrix();
    glm::mat4 view       = MyCamera.GetViewMatrix();

    glm::mat4 modelBox = glm::mat4(1.f);
    // glm::translate/rotate/scale都是model*T/R/S，在OpenGL中矩阵按列存储，顶点是列向量，要右乘，因此最后添加的变换先应用
    modelBox = glm::translate(modelBox, BoxTranslate);
    modelBox = glm::rotate(modelBox, glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f)); // angle单位是弧度，
    modelBox = glm::scale(modelBox, glm::vec3(BoxScale));

    BoxShader->Bind();
    BoxShader->SetUniformMat4f("uModel", modelBox);
    BoxShader->SetUniformMat4f("uView", view);
    BoxShader->SetUniformMat4f("uProjection", projection);
    BoxShader->SetUniform3f("uLightColor", LightColor);
    BoxShader->SetUniform3f("uLightPos", LightPos);
    BoxShader->SetUniform3f("uObjectColor", BoxColor);
    BoxShader->SetUniform3f("uCameraPos", MyCamera.GetPosition());

    Renderer renderer;
    renderer.Draw(*BoxVAO, *BoxShader);

    glm::mat4 modelLight = glm::translate(glm::mat4(1.f), LightTranslate);
    modelLight           = glm::scale(modelLight, glm::vec3(10.f));

    // 绘制光源立方体，便于确认光源位置
    LightShader->Bind();
    LightShader->SetUniformMat4f("uModel", modelLight);
    LightShader->SetUniformMat4f("uView", view);
    LightShader->SetUniformMat4f("uProjection", projection);
    LightShader->SetUniform3f("uLightColor", LightColor);

    renderer.Draw(*LightVAO, *LightShader);
}

void TestDrawCube::OnImGuiRender()
{
    // 光源设置
    ImGui::Text("光源设置");
    ImGui::Separator();
    ImGui::SliderFloat3("光源平移量", &LightTranslate[0], -100.f, 100.f, "%.2f");
    ImGui::ColorEdit3("光源颜色", &LightColor[0]);

    // 立方体设置
    ImGui::Text("立方体设置");
    ImGui::Separator();
    ImGui::SliderFloat3("立方体平移量", &BoxTranslate[0], -100.f, 100.f, "%.2f");
    ImGui::SliderFloat("立方体缩放", &BoxScale, 0.001f, 100.f, "%.2f");
    ImGui::ColorEdit3("立方体颜色", &BoxColor[0]);
}


NAMESPACE_END(test)
