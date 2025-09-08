#include "renderer.h"

#include "GL/glew.h"

#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::Clear() const
{
    // 在没绑定帧缓冲时，清理的是后缓冲区的颜色数据
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray &VAO, const Shader &shader) const
{
    // 在OpenGL中事实上不需要unbind操作，有如下三点原因
    // 1. OpenGL是State Machine
    // OpenGL内部维护着一堆全局状态，例如当前绑定的VAO，VBO， Program
    // 在绑定一个新的对象时，之前的对象会被自动替换，没有必要先unbind
    // 2. OpenGL 不会“悬挂引用”。如果你删除了某个对象（比如 glDeleteBuffers），即使它当前绑定着，OpenGL
    // 也会在内部延迟到安全时机再释放，不会导致悬挂指针那样的问题。
    // 3. 解绑（glBindBuffer(..., 0)）本质上就是一次状态切换，频繁unbind反而会增加状态改变的次数
    VAO.Bind();
    shader.Bind();

    // 根据是否有索引缓冲采用不同的绘制方式
    if (VAO.HasIndexBuffer())
    {
        glDrawElements(GL_TRIANGLES, VAO.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, VAO.GetVertexCount());
    }
}
