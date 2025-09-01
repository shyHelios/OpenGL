#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::unordered_map<unsigned int, std::string> &shader_files);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string &name, int value);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);

    void SetUniform1iv(const std::string &name, int count, int *values);

    void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

private:
    int GetUniformLocation(const std::string &name) const;

    /**
     * @brief 从指定文件加载shader源代码
     * 
     * @param filepath 文件路径
     * @return std::string shader源代码
     */
    static std::string LoadShaderSource(const std::string &filepath);

    /**
     * @brief 编译指定类型的shader，
     *
     * @param type shader类型，例如vertex shader、fragment shader等
     * @param source_code shader源代码
     * @return unsigned int 返回0表示编译失败，否则会返回shader id
     */
    unsigned int CompileShader(unsigned int type, const std::string &source_code);

    unsigned int m_renderer_id;

    // 顶点、片段、几何、细分着色器的源代码，从GLenum映射到源代码
    std::unordered_map<unsigned int, std::string> m_shader_sources;
    mutable std::unordered_map<std::string, int> m_uniform_location_cache;
};
