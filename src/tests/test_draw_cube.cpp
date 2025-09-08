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
    // 物体vao
    BoxVAO = std::make_unique<VertexArray>();

    float vertices[] = {
            -10.f, -10.f, -10.f, 0.0f, 0.0f, 10.f,  -10.f, -10.f, 1.0f, 0.0f, 10.f,  10.f,  -10.f, 1.0f, 1.0f,
            10.f,  10.f,  -10.f, 1.0f, 1.0f, -10.f, 10.f,  -10.f, 0.0f, 1.0f, -10.f, -10.f, -10.f, 0.0f, 0.0f,

            -10.f, -10.f, 10.f,  0.0f, 0.0f, 10.f,  -10.f, 10.f,  1.0f, 0.0f, 10.f,  10.f,  10.f,  1.0f, 1.0f,
            10.f,  10.f,  10.f,  1.0f, 1.0f, -10.f, 10.f,  10.f,  0.0f, 1.0f, -10.f, -10.f, 10.f,  0.0f, 0.0f,

            -10.f, 10.f,  10.f,  1.0f, 0.0f, -10.f, 10.f,  -10.f, 1.0f, 1.0f, -10.f, -10.f, -10.f, 0.0f, 1.0f,
            -10.f, -10.f, -10.f, 0.0f, 1.0f, -10.f, -10.f, 10.f,  0.0f, 0.0f, -10.f, 10.f,  10.f,  1.0f, 0.0f,

            10.f,  10.f,  10.f,  1.0f, 0.0f, 10.f,  10.f,  -10.f, 1.0f, 1.0f, 10.f,  -10.f, -10.f, 0.0f, 1.0f,
            10.f,  -10.f, -10.f, 0.0f, 1.0f, 10.f,  -10.f, 10.f,  0.0f, 0.0f, 10.f,  10.f,  10.f,  1.0f, 0.0f,

            -10.f, -10.f, -10.f, 0.0f, 1.0f, 10.f,  -10.f, -10.f, 1.0f, 1.0f, 10.f,  -10.f, 10.f,  1.0f, 0.0f,
            10.f,  -10.f, 10.f,  1.0f, 0.0f, -10.f, -10.f, 10.f,  0.0f, 0.0f, -10.f, -10.f, -10.f, 0.0f, 1.0f,

            -10.f, 10.f,  -10.f, 0.0f, 1.0f, 10.f,  10.f,  -10.f, 1.0f, 1.0f, 10.f,  10.f,  10.f,  1.0f, 0.0f,
            10.f,  10.f,  10.f,  1.0f, 0.0f, -10.f, 10.f,  10.f,  0.0f, 0.0f, -10.f, 10.f,  -10.f, 0.0f, 1.0f};

    std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    VertexBufferLayout boxLayout;
    boxLayout.PushAttribute<float>(3);
    boxLayout.PushAttribute<float>(2);
    BoxVAO->AddBuffer(vertexBuffer, boxLayout);

    // 光源vao
    LightVAO = std::make_unique<VertexArray>();
    VertexBufferLayout lightLayout;
    lightLayout.PushAttribute<float>(3);
    lightLayout.PushAttribute<float>(2);
    LightVAO->AddBuffer(vertexBuffer, lightLayout);

    std::unordered_map<unsigned int, std::string> boxShaderFiles{
            {GL_VERTEX_SHADER,   "resources/shaders/test_draw_cube/cube.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/test_draw_cube/cube.frag"}
    };
    BoxShader = std::make_unique<Shader>(boxShaderFiles);

    std::unordered_map<unsigned int, std::string> lightShaderFiles{
            {GL_VERTEX_SHADER,   "resources/shaders/test_draw_cube/light.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/test_draw_cube/light.frag"},
    };
    LightShader = std::make_unique<Shader>(lightShaderFiles);

    m_Texture = std::make_unique<Texture>("resources/dog.png");
}

TestDrawCube::~TestDrawCube() {}

void TestDrawCube::OnUpdate(float delta_time) {}

void TestDrawCube::OnRender()
{
    glm::mat4 projection = MyCamera.GetProjectionMatrix();
    glm::mat4 view       = MyCamera.GetViewMatrix();
    glm::mat4 modelBox   = glm::translate(glm::mat4(1.f), BoxTranslate);

    BoxShader->Bind();
    BoxShader->SetUniformMat4f("u_Model", modelBox);
    BoxShader->SetUniformMat4f("u_View", view);
    BoxShader->SetUniformMat4f("u_Projection", projection);
    BoxShader->SetUniform1i("u_Texture", 0);
    BoxShader->SetUniform4f("u_LightColor", glm::vec4(1.f, 1.f, 1.f, 1.f));

    Renderer renderer;
    renderer.Draw(*BoxVAO, *BoxShader);

    glm::mat4 modelLight = glm::translate(glm::mat4(1.f), LightTranslate);

    LightShader->Bind();
    LightShader->SetUniformMat4f("u_Model", modelLight);
    LightShader->SetUniformMat4f("u_View", view);
    LightShader->SetUniformMat4f("u_Projection", projection);

    renderer.Draw(*LightVAO, *LightShader);
}

void TestDrawCube::OnImGuiRender()
{
    ImGui::SliderFloat3("光源平移量", &LightTranslate[0], -100.f, 100.f, "%.2f");
    ImGui::SliderFloat3("立方体平移量", &BoxTranslate[0], -100.f, 100.f, "%.2f");
}


NAMESPACE_END(test)
