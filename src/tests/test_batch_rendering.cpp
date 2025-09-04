#include "test_batch_rendering.h"

#include <array>
#include <memory>

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

struct Vertex
{
    std::array<float, 2> position;   // 位置
    std::array<float, 3> color;      // 颜色
    std::array<float, 2> tex_coords; // 纹理坐标
    float tex_id;                    // 纹理id
};

std::array<Vertex, 4> CreateQuad(const glm::vec2 &pos, float size, float tex_id)
{
    float x = pos.x;
    float y = pos.y;

    Vertex v0;
    v0.position   = {x, y};
    v0.color      = {1.f, 0.f, 0.f};
    v0.tex_coords = {0.f, 0.f};
    v0.tex_id     = tex_id;

    Vertex v1;
    v1.position   = {x + size, y};
    v1.color      = {1.f, 0.f, 0.f};
    v1.tex_coords = {1.f, 0.f};
    v1.tex_id     = tex_id;

    Vertex v2;
    v2.position   = {x + size, y + size};
    v2.color      = {1.f, 0.f, 0.f};
    v2.tex_coords = {1.f, 1.f};
    v2.tex_id     = tex_id;

    Vertex v3;
    v3.position   = {x, y + size};
    v3.color      = {1.f, 0.f, 0.f};
    v3.tex_coords = {0.f, 1.f};
    v3.tex_id     = tex_id;

    return {v0, v1, v2, v3};
}

TestBatchRendering::TestBatchRendering() :
    m_vao(),
    m_shader(),
    m_translation_a(100.f, 200.f, 0.f)
{
    // 创建vao，核心模式下不创建无法绘制
    // vao只会在调用glVertexAttribPointer时记录VBO的绑定情况
    m_vao = std::make_unique<VertexArray>();

    const size_t MaxQuadCount   = 1000;
    const size_t MaxVertexCount = MaxQuadCount * 4;
    const size_t MaxIndexCount  = MaxQuadCount * 6;

    // 动态修改GPU中的数据，数据指针设为nullptr只会分配空间，不会拷贝数据，
    // 设置为GL_DYNAMIC_DRAW表明我们会经常修改其中的数据
    auto vbo = std::make_shared<VertexBuffer>(nullptr, 1000 * sizeof(Vertex), GL_DYNAMIC_DRAW);

    std::array<Vertex, 3> test;

    std::array<Vertex, 4> q0 = CreateQuad(glm::vec2(-50.f, -50.f), 100.f, 0.f);
    std::array<Vertex, 4> q1 = CreateQuad(glm::vec2(100.f, -50.f), 100.f, 1.f);
    Vertex vertices[8];
    memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));
    memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

    vbo->SubData(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.PushAttribute<float>(2); // 位置坐标
    layout.PushAttribute<float>(3); // 颜色
    layout.PushAttribute<float>(2); // 纹理坐标
    layout.PushAttribute<float>(1); // 纹理id

    m_vao->AddBuffer(vbo, layout);

    uint32_t indices[MaxIndexCount];
    uint32_t offset = 0;
    for (size_t i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    auto ibo = std::make_shared<IndexBuffer>(indices, 12);
    m_vao->AddBuffer(ibo);

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
    renderer.Draw(*m_vao, *m_shader);
}

void TestBatchRendering::OnImGuiRender()
{
    float x_max = static_cast<float>(window_width);
    float y_max = static_cast<float>(window_height);
    ImGui::SliderFloat3("translation", &m_translation_a[0], 0.f, x_max);
}

NAMESPACE_END(test)
