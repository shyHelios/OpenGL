#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "index_buffer.h"
#include "vertex_buffer.h"

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR) {};
}

static std::string ParseShader(const std::string &filepath)
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

static unsigned int CompileShader(unsigned int type, const std::string &source_code)
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
        std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string &vertex_shader, const std::string &fragment_shader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs      = CompileShader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs      = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // 开启opengl核心模式, 移除固定管线和API
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        std::getchar();
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // 垂直同步
    // 我们可以把显示器理解为一个只知道以固定速率从显存的前台缓冲区中取数据绘制的工人
    // 显卡是以帧为单位生成画面的，生成的数据在后台缓冲区，生成之后调用glfwSwapBuffers交换前台缓冲区和后台缓冲区
    // glfwSwapInterval(0)表示显卡生成完一帧画面后不等待，直接交换缓冲区，显示器前一帧可能还没渲染完
    // 就从下一帧中取数据了，因此会出现画面撕裂
    // glfwSwapInterval(1)表示显卡生成完后等待一次显示器的垂直消隐，即绘制完一帧后的空档，再交换缓冲区，这样能保证前一帧
    // 渲染完后才会绘制当前帧
    // glfwSwapInterval(2)表示显卡生成完后等待两次显示器的垂直消隐，这样显示器会显示同一帧两次，计算帧率（每秒显示的不同的画面个数）
    // 会是显示器刷新率的一半
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW init error!" << std::endl;
    }

    // 创建vao，核心模式下不创建无法绘制
    // vao只会在调用glVertexAttribPointer时记录VBO的绑定情况
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float positions[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    vb.Bind();

    // ibo
    unsigned int indices[] = {
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
    };
    IndexBuffer ib(indices, 6);
    ib.Bind();

    glEnableVertexAttribArray(0); // 启用顶点属性0，如果还有法线、UV坐标等，依次为1、2
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *) 0);

    // 解绑vao，防止误操作
    glBindVertexArray(vao);

    std::string vert_shader_code = ParseShader("resources/shaders/basic.vert");
    std::string frag_shader_code = ParseShader("resources/shaders/basic.frag");

    unsigned int program = CreateShader(vert_shader_code, frag_shader_code);
    glUseProgram(program);

    int location = glGetUniformLocation(program, "u_color");
    if (location == -1)
    {
        std::cout << "Get uniform location error!" << std::endl;
    }

    float r         = 0.f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform4f(location, r, 0.5f, 0.8f, 1.f);
        r += increment;
        if (r > 1.f)
            r = 0.f;

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // 销毁OpenGL上下文
    glfwTerminate();
    return 0;
}
