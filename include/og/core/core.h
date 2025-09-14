#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>

#define NAMESPACE_BEGIN(name) \
namespace name {

#define NAMESPACE_END(name) }

template <typename T, int size>
std::string ToString(const glm::vec<size, T, glm::defaultp>& v)
{
    std::stringstream ss;
    ss << "[";
    ss << v[0];
    for (int i = 1; i < size; i++)
    {
        ss << ", ";
        ss << v[i];
    }
    ss << "]";
    return ss.str();
}