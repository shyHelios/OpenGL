#pragma once

#include <string>

#include <og/core/core.h>
#include <og/tests/test.h>

NAMESPACE_BEGIN(test)

class TestClearColor : public Test
{
public:
    TestClearColor(const std::string &InDisplayName);
    ~TestClearColor();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
private:
    float m_clear_color[4];
    float m_original_clear_color[4];
};

NAMESPACE_END(test)
