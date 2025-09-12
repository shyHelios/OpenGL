#include "test_lighting_maps.h"

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

TestLightingMaps::TestLightingMaps(const std::string& InDisplayName) :
    Test(InDisplayName)
{
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置相机pose
    MyCamera.SetPosition(glm::vec3(46.1923f, -13.9571f, -23.4756f));
    MyCamera.SetPitch(5.1f);
    MyCamera.SetYaw(-202.5f);

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
            {GL_VERTEX_SHADER,   "resources/lighting_maps/shaders/box.vert"},
            {GL_FRAGMENT_SHADER, "resources/lighting_maps/shaders/box.frag"}
    };
    BoxShader = std::make_unique<Shader>(boxShaderFiles);

    std::unordered_map<unsigned int, std::string> lightShaderFiles{
            {GL_VERTEX_SHADER,   "resources/lighting_maps/shaders/light.vert"},
            {GL_FRAGMENT_SHADER, "resources/lighting_maps/shaders/light.frag"}
    };
    LightShader = std::make_unique<Shader>(lightShaderFiles);

    BoxDiffuseTexture = std::make_unique<Texture>("resources/lighting_maps/textures/container2.png", 0);
    BoxSpecularTexture = std::make_unique<Texture>("resources/lighting_maps/textures/container2_specular.png", 1);
}

TestLightingMaps::~TestLightingMaps()
{
    glDisable(GL_DEPTH_TEST);
}

void TestLightingMaps::OnUpdate(float delta_time)
{
    LightPos = InitialLightPos + LightTranslate;
}

void TestLightingMaps::OnRender()
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
    BoxShader->SetUniform3f("uCameraPos", MyCamera.GetPosition());

    BoxShader->SetUniform1i("uMaterial.diffuse", 0);
    BoxShader->SetUniform1i("uMaterial.specular", 1);
    BoxShader->SetUniform1f("uMaterial.shininess", BoxShininess);

    BoxShader->SetUniform3f("uLight.position", LightPos);
    BoxShader->SetUniform3f("uLight.diffuse", LightDiffuse);
    BoxShader->SetUniform3f("uLight.specular", LightSpecular);

    Renderer renderer;
    renderer.Draw(*BoxVAO, *BoxShader);

    glm::mat4 modelLight = glm::translate(glm::mat4(1.f), LightTranslate);
    modelLight           = glm::scale(modelLight, glm::vec3(10.f));

    // 绘制光源立方体，便于确认光源位置
    LightShader->Bind();
    LightShader->SetUniformMat4f("uModel", modelLight);
    LightShader->SetUniformMat4f("uView", view);
    LightShader->SetUniformMat4f("uProjection", projection);
    LightShader->SetUniform3f("uLightColor", glm::vec3(1.f, 1.f, 1.f));

    renderer.Draw(*LightVAO, *LightShader);
}

void TestLightingMaps::OnImGuiRender()
{
    // 光源设置
    ImGui::Text("光源设置");
    ImGui::Separator();
    ImGui::DragFloat3("光源平移量", &LightTranslate[0], TransformDragSpeed, -100.f, 100.f, "%.2f");
    ImGui::ColorEdit3("漫反射光源颜色", &LightDiffuse[0], ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("镜面反射光源颜色", &LightSpecular[0], ImGuiColorEditFlags_Float);

    // 立方体设置
    ImGui::Text("立方体设置");
    ImGui::Separator();
    ImGui::DragFloat3("立方体平移量", &BoxTranslate[0], TransformDragSpeed, -100.f, 100.f, "%.2f");
    ImGui::DragFloat("立方体缩放", &BoxScale, TransformDragSpeed, 0.001f, 100.f, "%.2f");
    // 材质
    ImGui::DragFloat("高光度", &BoxShininess, TransformDragSpeed, 1.f, 64.f, "%.1f");
}


NAMESPACE_END(test)
