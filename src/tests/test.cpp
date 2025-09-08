#include "test.h"

#include <imgui.h>

#include "../core.h"

NAMESPACE_BEGIN(test)

TestMenu::TestMenu(Test *&current_test) : Test("测试列表"), m_current_test(current_test) {}

void TestMenu::OnImGuiRender() {
    for (auto &[test_name, create_test_func] : m_tests)
    {
        if (ImGui::Selectable(test_name.c_str(), false))
        {
            m_current_test = create_test_func();
        }
    }
}

NAMESPACE_END(test)
