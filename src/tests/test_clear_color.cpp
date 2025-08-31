#include "test_clear_color.h"

#include <GL/glew.h>
#include <imgui.h>

NAMESPACE_BEGIN(test)

TestClearColor::TestClearColor() : m_clear_color{0.2f, 0.3f, 0.8f, 1.f}
{
    glGetFloatv(GL_COLOR_CLEAR_VALUE, m_original_clear_color);
}

TestClearColor::~TestClearColor()
{
    glClearColor(m_original_clear_color[0], m_original_clear_color[1], m_original_clear_color[2],
                 m_original_clear_color[3]);
}

void TestClearColor::OnUpdate(float delta_time) {}

void TestClearColor::OnRender()
{
    glClearColor(m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void TestClearColor::OnImGuiRender()
{
    ImGui::ColorEdit4("Clear Color", m_clear_color);
}

NAMESPACE_END(test)
