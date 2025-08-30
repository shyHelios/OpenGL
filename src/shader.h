#pragma once

#include <string>
#include <unordered_map>

class Shader
{
public:
    Shader();
    ~Shader();

    void Configure(const std::unordered_map<unsigned int, std::string> &shader_files);

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string &name, int value);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);

private:
    int GetUniformLocation(const std::string &name);

    /**
     * @brief 从指定文件加载shader源代码
     * 
     * @param filepath 文件路径
     * @return std::string shader源代码
     */
    std::string LoadShaderSource(const std::string &filepath);

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
};
