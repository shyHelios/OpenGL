#pragma once

#include "../core.h"
#include "test.h"

NAMESPACE_BEGIN(test)

class TestClearColor : public Test
{
public:
    TestClearColor();
    ~TestClearColor();

    virtual void OnUpdate(float delta_time) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
private:
    float m_clear_color[4];
    float m_original_clear_color[4];
};

NAMESPACE_END(test)
