#include "test_texture_2d.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "../core.h"
#include "../index_buffer.h"
#include "../renderer.h"
#include "../shader.h"
#include "../texture.h"
#include "../vertex_array.h"
#include "../vertex_buffer.h"
#include "../vertex_buffer_layout.h"

extern int window_width;
extern int window_height;

NAMESPACE_BEGIN(test)

TestTexture2D::TestTexture2D() :
    m_vao(),
    m_vbo(),
    m_shader(),
    m_translation_a(100.f, 200.f, 0.f),
    m_translation_b(100.f, 100.f, 0.f)
{
    // 创建vao，核心模式下不创建无法绘制
    // vao只会在调用glVertexAttribPointer时记录VBO的绑定情况
    m_vao = std::make_unique<VertexArray>();
    m_vao->Bind();

    // 模型空间的各个顶点位置
    float positions[] = {
            -50.f, -50.f, 0.f, 0.f, // 0
            50.f,  -50.f, 1.f, 0.f, // 1
            50.f,  50.f,  1.f, 1.f, // 2
            -50.f, 50.f,  0.f, 1.f  // 3
    };
    m_vbo = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
    m_vbo->Bind();

    VertexBufferLayout layout;
    layout.PushAttribute<float>(2); // 位置坐标
    layout.PushAttribute<float>(2); // 纹理坐标
    m_vao->AddBuffer(*m_vbo, layout);

    // ibo
    unsigned int indices[] = {
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
    };
    m_ibo = std::make_unique<IndexBuffer>(indices, 6);
    m_ibo->Bind();

    m_texture = std::make_unique<Texture>("resources/cats.png");
    m_texture->Bind();

    // 创建shader
    const std::unordered_map<unsigned int, std::string> shader_files{
            {GL_VERTEX_SHADER,   "resources/shaders/basic.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/basic.frag"},
    };
    m_shader = std::make_unique<Shader>(shader_files);
    m_shader->Bind();
    m_shader->SetUniform1i("u_Texture", 0);
}

TestTexture2D::~TestTexture2D() {}

void TestTexture2D::OnUpdate(float delta_time) {}

void TestTexture2D::OnRender()
{
    glm::mat4 proj  = glm::ortho(0.f, 640.f, 0.f, 480.f, -1.f, 1.f);
    glm::mat4 model = glm::translate(glm::mat4(1.f), m_translation_a);
    glm::mat4 mvp_a = proj * model;

    model           = glm::translate(glm::mat4(1.f), m_translation_b);
    glm::mat4 mvp_b = proj * model;

    Renderer renderer;
    m_shader->SetUniformMat4f("u_MVP", mvp_a);
    renderer.Draw(*m_vao, *m_ibo, *m_shader);

    m_shader->SetUniformMat4f("u_MVP", mvp_b);
    renderer.Draw(*m_vao, *m_ibo, *m_shader);
}

void TestTexture2D::OnImGuiRender()
{
    float x_max = static_cast<float>(window_width);
    float y_max = static_cast<float>(window_height);
    ImGui::SliderFloat3("translation a", &m_translation_a[0], 0.f, x_max);
    ImGui::SliderFloat3("translation b", &m_translation_b[0], 0.f, x_max);
}

NAMESPACE_END(test)
