#pragma once

#include <vector>

#include <GL/glew.h>


/**
 * @brief 每个顶点可能有多个属性，如位置，法向，纹理坐标等，
 * 该类作为顶点属性的抽象。
 */
struct VertexAttribute
{
    // 该属性有多少个component，如位置属性使用3个component描述
    unsigned int count;
    // 该属性的每个component的类型，如位置属性的每个component是一个float类型
    unsigned int type;
    // 输入非浮点类型值时是否需要归一化，即有符号数归到[-1, 1]，无符号数归到[0, 1]
    unsigned int normalized;
    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT:
                return 4;
                /* code */
        }
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() {}
    ~VertexBufferLayout() {}

    template<typename T> void PushAttribute(unsigned int count) { static_assert(false); }

    template<> void PushAttribute<float>(unsigned int count)
    {
        m_attributes.emplace_back(count, GL_FLOAT, false);
        m_stride += count * VertexAttribute::GetSizeOfType(GL_FLOAT);
    }

    template<> void PushAttribute<unsigned int>(unsigned int count)
    {
        m_attributes.emplace_back(count, GL_UNSIGNED_INT, false);
        m_stride += count * VertexAttribute::GetSizeOfType(GL_UNSIGNED_INT);
    }

    const std::vector<VertexAttribute> &GetAttributes() const { return m_attributes; }
    unsigned int GetStride() const { return m_stride; }

private:
    // 存储一个顶点的所有属性
    std::vector<VertexAttribute> m_attributes;
    // buffer中从当前顶点跨到下一个顶点所需字节数
    unsigned int m_stride = 0;
};
