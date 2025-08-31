#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>

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

class TestMenu : public Test
{
public:
    TestMenu(Test *&current_test);
    ~TestMenu() {};

    template<typename T> void RegisterTest(const std::string &test_name)
    {
        std::cout << "Registering Test: " << test_name << std::endl;
        m_tests[test_name] = []() { return new T(); };
    }

    void OnImGuiRender() override;

private:
    Test *&m_current_test;
    std::unordered_map<std::string, std::function<Test *()>> m_tests;
};

NAMESPACE_END(test)
