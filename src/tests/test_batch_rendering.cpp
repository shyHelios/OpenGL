#include "test_batch_rendering.h"

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

TestBatchRendering::TestBatchRendering() :
    m_vao(),
    m_vbo(),
    m_shader(),
    m_translation_a(100.f, 200.f, 0.f)
{
    // 创建vao，核心模式下不创建无法绘制
    // vao只会在调用glVertexAttribPointer时记录VBO的绑定情况
    m_vao = std::make_unique<VertexArray>();
    // m_vao->Bind();

    // 模型空间的各个顶点位置
    float positions[] = {
            -50.f, -50.f, 1.f,  0.f,  0.f,  0.f, 0.f, 0.f, // 0
            50.f,  -50.f, 1.f,  0.f,  0.f,  1.f, 0.f, 0.f, // 1
            50.f,  50.f,  1.f,  0.f,  0.f,  1.f, 1.f, 0.f, // 2
            -50.f, 50.f,  1.f,  0.f,  0.f,  0.f, 1.f, 0.f, // 3

            100.f, -50.f, 0.2f, 0.3f, 0.8f, 0.f, 0.f, 1.f, // 4
            200.f, -50.f, 0.2f, 0.3f, 0.8f, 1.f, 0.f, 1.f, // 5
            200.f, 50.f,  0.2f, 0.3f, 0.8f, 1.f, 1.f, 1.f, // 6
            100.f, 50.f,  0.2f, 0.3f, 0.8f, 0.f, 1.f, 1.f  // 7
    };
    m_vbo = std::make_unique<VertexBuffer>(positions, 8 * 8 * sizeof(float));
    VertexBufferLayout layout;
    layout.PushAttribute<float>(2); // 位置坐标
    layout.PushAttribute<float>(3); // 颜色
    layout.PushAttribute<float>(2); // 纹理坐标
    layout.PushAttribute<float>(1); // 纹理id

    m_vao->AddBuffer(*m_vbo, layout);

    // ibo
    unsigned int indices[] = {
            0, 1, 2, // 第一个三角形
            2, 3, 0, // 第二个三角形

            4, 5, 6, // 第三个三角形
            6, 7, 4  // 第四个三角形
    };
    m_ibo = std::make_unique<IndexBuffer>(indices, 12);
    m_vao->AddBuffer(*m_ibo);

    // 创建shader
    const std::unordered_map<unsigned int, std::string> shader_files{
            {GL_VERTEX_SHADER,   "resources/shaders/batch_color.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/batch_color.frag"},
    };

    m_texture_0 = std::make_unique<Texture>("resources/cat.png", 0);

    m_texture_1 = std::make_unique<Texture>("resources/dog.png", 1);

    m_shader = std::make_unique<Shader>(shader_files);
    m_shader->Bind();
    int samplers[] = {0, 1};
    m_shader->SetUniform1iv("u_Texture", 2, samplers);
}

TestBatchRendering::~TestBatchRendering() {}

void TestBatchRendering::OnUpdate(float delta_time) {}

void TestBatchRendering::OnRender()
{
    glm::mat4 proj  = glm::ortho(0.f, 640.f, 0.f, 480.f, -1.f, 1.f);
    glm::mat4 model = glm::translate(glm::mat4(1.f), m_translation_a);
    glm::mat4 mvp_a = proj * model;

    Renderer renderer;
    m_shader->SetUniformMat4f("u_MVP", mvp_a);
    renderer.Draw(*m_vao, *m_ibo, *m_shader);
}

void TestBatchRendering::OnImGuiRender()
{
    float x_max = static_cast<float>(window_width);
    float y_max = static_cast<float>(window_height);
    ImGui::SliderFloat3("translation", &m_translation_a[0], 0.f, x_max);
}

NAMESPACE_END(test)
