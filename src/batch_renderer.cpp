#include "batch_renderer.h"

#include <array>

#include <GL/glew.h>

static const size_t MaxQuadCount   = 1000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount  = MaxQuadCount * 6;
static const size_t MaxTextures    = 32;

struct Vertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoords;
    float TexIdx;
};

struct RendererData
{
    uint32_t QuadVA = 0; // vertex array
    uint32_t QuadVB = 0; // vertex buffer
    uint32_t QuadIB = 0; // index buffer

    uint32_t WhiteTexture     = 0; // white texture的id
    uint32_t WhiteTextureSlot = 0;

    uint32_t IndexCount = 0;

    Vertex *QuadBuffer    = nullptr; // 包含CPU端要绘制的顶点数据的指针
    Vertex *QuadBufferPtr = nullptr;

    std::array<uint32_t, MaxTextures> TextureSlots; // 存储每个slot映射到的texture id，数组下标就是key
    uint32_t TextureSlotIndex = 1;                  // 下一张纹理可以存放的纹理槽

    BatchRenderer::Stats RenderStats;
};

static RendererData s_Data;

void BatchRenderer::Init()
{
    s_Data.QuadBuffer = new Vertex[MaxVertexCount];

    glCreateVertexArrays(1, &s_Data.QuadVA);
    glBindVertexArray(s_Data.QuadVA);

    glCreateBuffers(1, &s_Data.QuadVB);
    glNamedBufferStorage(s_Data.QuadVB, MaxVertexCount * sizeof(Vertex), nullptr,
                         GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
    glVertexArrayVertexBuffer(s_Data.QuadVA, 0, s_Data.QuadVB, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(s_Data.QuadVA, 0);
    // 把attribute i映射到binding point 0
    glVertexArrayAttribBinding(s_Data.QuadVA, 0, 0);
    // 定义attribute数据格式
    glVertexArrayAttribFormat(s_Data.QuadVA, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));

    glEnableVertexArrayAttrib(s_Data.QuadVA, 1);
    glVertexArrayAttribBinding(s_Data.QuadVA, 1, 0);
    glVertexArrayAttribFormat(s_Data.QuadVA, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, Color));

    glEnableVertexArrayAttrib(s_Data.QuadVA, 2);
    glVertexArrayAttribBinding(s_Data.QuadVA, 2, 0);
    glVertexArrayAttribFormat(s_Data.QuadVA, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));

    glEnableVertexArrayAttrib(s_Data.QuadVA, 3);
    glVertexArrayAttribBinding(s_Data.QuadVA, 3, 0);
    glVertexArrayAttribFormat(s_Data.QuadVA, 3, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexIdx));

    uint32_t indices[MaxIndexCount];
    uint32_t offset = 0;
    for (size_t i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    glCreateBuffers(1, &s_Data.QuadIB);
    glNamedBufferData(s_Data.QuadIB, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(s_Data.QuadVA, s_Data.QuadIB);

    // 1x1 white texture
    glCreateTextures(GL_TEXTURE_2D, 1, &s_Data.WhiteTexture);
    glTextureParameteri(s_Data.WhiteTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(s_Data.WhiteTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(s_Data.WhiteTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(s_Data.WhiteTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uint32_t color = 0xffffffff;
    glTextureStorage2D(s_Data.WhiteTexture, 1, GL_RGBA8, 1, 1); // 分配存储（现代 DSA，替代 glTexImage2D）
    glTextureSubImage2D(s_Data.WhiteTexture, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                        &color); // 上传像素数据

    s_Data.TextureSlots[0] = s_Data.WhiteTexture;
    for (size_t i = 1; i < MaxTextures; i++)
    {
        s_Data.TextureSlots[i] = 0;
    }
}

void BatchRenderer::Shutdown()
{
    glDeleteVertexArrays(1, &s_Data.QuadVA);
    glDeleteBuffers(1, &s_Data.QuadVB);
    glDeleteBuffers(1, &s_Data.QuadIB);

    // 删除white texture
    glDeleteTextures(1, &s_Data.WhiteTexture);

    delete[] s_Data.QuadBuffer;
}

void BatchRenderer::BeginBatch()
{
    s_Data.QuadBufferPtr = s_Data.QuadBuffer;
}

void BatchRenderer::EndBatch()
{
    GLsizeiptr size = (s_Data.QuadBufferPtr - s_Data.QuadBuffer) * sizeof(Vertex);
    glNamedBufferSubData(s_Data.QuadVB, 0, size, s_Data.QuadBuffer);
}

void BatchRenderer::Flush()
{
    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
    {
        // FIXME: 检查在之前有没有调用glCreateTextures
        glBindTextureUnit(i, s_Data.TextureSlots[i]);
    }

    // 绘制前再次绑定
    glBindVertexArray(s_Data.QuadVA);
    glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);
    s_Data.RenderStats.DrawCount++;

    // ????
    s_Data.IndexCount = 0;
    s_Data.TextureSlotIndex = 0;
}
