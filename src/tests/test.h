#pragma once

#include "../core.h"

NAMESPACE_BEGIN(test)

class Test
{
public:
    Test() {}
    virtual ~Test() {}

    virtual void OnUpdate(float delta_time) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}
};

NAMESPACE_END(test)
