#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../core.h"

NAMESPACE_BEGIN(test)

class Test
{
public:
    Test(const std::string InName) :
        Name(InName)
    {}
    virtual ~Test() {}

    std::string GetName() const
    {
        return Name;
    }

    virtual void OnUpdate(float delta_time) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}

    std::string Name;
};

class TestMenu : public Test
{
public:
    TestMenu(Test*& current_test);
    ~TestMenu() {};

    template<typename T> void RegisterTest(const std::string& test_name)
    {
        std::cout << "Registering Test: " << test_name << std::endl;
        m_tests[test_name] = []() { return new T(); };
    }

    void OnImGuiRender() override;

private:
    Test*& m_current_test;
    std::unordered_map<std::string, std::function<Test*()>> m_tests;
};

NAMESPACE_END(test)
