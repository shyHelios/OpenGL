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
#include "../vertex_array.h"
#include "../vertex_buffer.h"
#include "../vertex_buffer_layout.h"
#include "../texture.h"

extern Camera MyCamera;

NAMESPACE_BEGIN(test)

TestDrawCube::TestDrawCube(const std::string& InDisplayName) :
    Test(InDisplayName)
{
    VAO = std::make_unique<VertexArray>();

    // float vertices[] = {
    //         -10.f, -10.f, -10.f, 0.0f, 0.0f, 10.f,  -10.f, -10.f, 1.0f, 0.0f, 10.f,  10.f,  -10.f, 1.0f, 1.0f,
    //         10.f,  10.f,  -10.f, 1.0f, 1.0f, -10.f, 10.f,  -10.f, 0.0f, 1.0f, -10.f, -10.f, -10.f, 0.0f, 0.0f,

    //         -10.f, -10.f, 10.f,  0.0f, 0.0f, 10.f,  -10.f, 10.f,  1.0f, 0.0f, 10.f,  10.f,  10.f,  1.0f, 1.0f,
    //         10.f,  10.f,  10.f,  1.0f, 1.0f, -10.f, 10.f,  10.f,  0.0f, 1.0f, -10.f, -10.f, 10.f,  0.0f, 0.0f,

    //         -10.f, 10.f,  10.f,  1.0f, 0.0f, -10.f, 10.f,  -10.f, 1.0f, 1.0f, -10.f, -10.f, -10.f, 0.0f, 1.0f,
    //         -10.f, -10.f, -10.f, 0.0f, 1.0f, -10.f, -10.f, 10.f,  0.0f, 0.0f, -10.f, 10.f,  10.f,  1.0f, 0.0f,

    //         10.f,  10.f,  10.f,  1.0f, 0.0f, 10.f,  10.f,  -10.f, 1.0f, 1.0f, 10.f,  -10.f, -10.f, 0.0f, 1.0f,
    //         10.f,  -10.f, -10.f, 0.0f, 1.0f, 10.f,  -10.f, 10.f,  0.0f, 0.0f, 10.f,  10.f,  10.f,  1.0f, 0.0f,

    //         -10.f, -10.f, -10.f, 0.0f, 1.0f, 10.f,  -10.f, -10.f, 1.0f, 1.0f, 10.f,  -10.f, 10.f,  1.0f, 0.0f,
    //         10.f,  -10.f, 10.f,  1.0f, 0.0f, -10.f, -10.f, 10.f,  0.0f, 0.0f, -10.f, -10.f, -10.f, 0.0f, 1.0f,

    //         -10.f, 10.f,  -10.f, 0.0f, 1.0f, 10.f,  10.f,  -10.f, 1.0f, 1.0f, 10.f,  10.f,  10.f,  1.0f, 0.0f,
    //         10.f,  10.f,  10.f,  1.0f, 0.0f, -10.f, 10.f,  10.f,  0.0f, 0.0f, -10.f, 10.f,  -10.f, 0.0f, 1.0f};

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
    VertexBufferLayout layout;
    layout.PushAttribute<float>(3);
    layout.PushAttribute<float>(2);
    VAO->AddBuffer(vertexBuffer, layout);

    std::unordered_map<unsigned int, std::string> shaderFiles{
            {GL_VERTEX_SHADER,   "resources/shaders/cube.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/cube.frag"}
    };
    m_Shader = std::make_unique<Shader>(shaderFiles);

    m_Texture = std::make_unique<Texture>("resources/dog.png");
}

TestDrawCube::~TestDrawCube() {}

void TestDrawCube::OnUpdate(float delta_time) {}

void TestDrawCube::OnRender()
{
    glm::mat4 projection = MyCamera.GetProjectionMatrix();
    glm::mat4 view       = MyCamera.GetViewMatrix();

    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_Projection", projection);
    m_Shader->SetUniformMat4f("u_View", view);
    m_Shader->SetUniform1i("u_Texture", 0);

    Renderer renderer;
    renderer.Draw(*VAO, *m_Shader);
}

void TestDrawCube::OnImGuiRender()
{
    ImGui::Text("暂时没有可调节的参数");
}


NAMESPACE_END(test)
