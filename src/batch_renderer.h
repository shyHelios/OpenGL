#pragma once

#include <glm/glm.hpp>


/**
 * @brief 2D Batch Renderer
 */
class BatchRenderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginBatch();
    // 把顶点缓冲区的数据上传至GPU
    static void EndBatch(); 
    static void Flush();

    static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
    static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, uint32_t texture_id);

    /**
     * @brief 统计数据，放在类内部，不污染全局命名空间
     */
    struct Stats
    {
        uint32_t DrawCount = 0;
        uint32_t QuadCount = 0;
    };
    static const Stats &GetStats();
    static void ResetStats();
};
