#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

Shader::Shader(const std::unordered_map<unsigned int, std::string> &shader_files) {
    for (const auto [type, filepath] : shader_files)
    {
        m_shader_sources[type] = LoadShaderSource(filepath);
    }
    m_renderer_id = glCreateProgram();

    for (const auto [type, source_code] : m_shader_sources)
    {
        unsigned int shader_id = CompileShader(type, source_code);
        glAttachShader(m_renderer_id, shader_id);
        glDeleteShader(shader_id);
    }

    glLinkProgram(m_renderer_id);
    glValidateProgram(m_renderer_id);
}

Shader::~Shader()
{
    glDeleteProgram(m_renderer_id);
}

void Shader::Bind() const
{
    glUseProgram(m_renderer_id);
}
void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string &name, int value)
{
    int location = GetUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
    int location = GetUniformLocation(name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::SetUniform1iv(const std::string &name, int count, int *values)
{
    int location = GetUniformLocation(name);
    glUniform1iv(location, count, values);
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix)
{
    int location = GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string &name) const
{
    if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
    {
        return m_uniform_location_cache[name];
    }

    int location = glGetUniformLocation(m_renderer_id, name.c_str());
    m_uniform_location_cache[name] = location;
    if (location == -1)
    {
        std::cout << "Get uniform location error!" << std::endl;
        return -1;
    }
    return location;
}

std::string Shader::LoadShaderSource(const std::string &filepath)
{
    std::ifstream stream(filepath);
    if (!stream.is_open())
    {
        std::cout << "Can't open file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source_code)
{
    unsigned int id = glCreateShader(type);
    const char *src = source_code.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader: " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}
