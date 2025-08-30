#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#

#include "index_buffer.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR) {};
}

// Debug 回调函数
void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam)
{
    std::string sourceStr;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API: sourceStr = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: sourceStr = "Other"; break;
    }

    std::string typeStr;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: typeStr = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "Undefined"; break;
        case GL_DEBUG_TYPE_PORTABILITY: typeStr = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: typeStr = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: typeStr = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: typeStr = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: typeStr = "Other"; break;
    }

    std::string severityStr;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: severityStr = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW: severityStr = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "NOTIFICATION"; break;
    }

    std::cerr << "[OpenGL Debug Message]"
              << " Source: " << sourceStr << " | Type: " << typeStr << " | ID: " << id << " | Severity: " << severityStr
              << "\n"
              << message << "\n"
              << std::endl;
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // 开启opengl核心模式, 移除固定管线和API
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    // 初始化glew之后才能使用OpenGL的函数，且初始化glew必须拥有一个OpenGL rendering上下文，
    // 即必须在glfwMakeContextCurrent后调用
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW init error!" << std::endl;
    }

    // 垂直同步
    // 我们可以把显示器理解为一个只知道以固定速率从显存的前台缓冲区中取数据绘制的工人
    // 显卡是以帧为单位生成画面的，生成的数据在后台缓冲区，生成之后调用glfwSwapBuffers交换前台缓冲区和后台缓冲区
    // glfwSwapInterval(0)表示显卡生成完一帧画面后不等待，直接交换缓冲区，显示器前一帧可能还没渲染完
    // 就从下一帧中取数据了，因此会出现画面撕裂
    // glfwSwapInterval(1)表示显卡生成完后等待一次显示器的垂直消隐，即绘制完一帧后的空档，再交换缓冲区，这样能保证前一帧
    // 渲染完后才会绘制当前帧
    // glfwSwapInterval(2)表示显卡生成完后等待两次显示器的垂直消隐，这样显示器会显示同一帧两次，计算帧率（每秒显示的不同的画面个数）
    // 会是显示器刷新率的一半
    glfwSwapInterval(5);

    // 启用调试输出
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glDebugMessageCallback(DebugCallback, nullptr);

    // 创建vao，核心模式下不创建无法绘制
    // vao只会在调用glVertexAttribPointer时记录VBO的绑定情况
    VertexArray va;
    va.Bind();

    float positions[] = {
            -0.5f, -0.5f, 0.f, 0.f, // 0
            0.5f,  -0.5f, 1.f, 0.f, // 1
            0.5f,  0.5f,  1.f, 1.f, // 2
            -0.5f, 0.5f,  0.f, 1.f  // 3
    };
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    vb.Bind();

    VertexBufferLayout layout;
    layout.PushAttribute<float>(2); // 位置坐标
    layout.PushAttribute<float>(2); // 纹理坐标
    va.AddBuffer(vb, layout);

    // ibo
    unsigned int indices[] = {
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
    };
    IndexBuffer ib(indices, 6);
    ib.Bind();

    // 创建shader
    Shader shader;
    shader.Configure({
            {GL_VERTEX_SHADER,   "resources/shaders/basic.vert"},
            {GL_FRAGMENT_SHADER, "resources/shaders/basic.frag"},
    });
    shader.Bind();


    Texture texture("resources/cats.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);

    float r         = 0.f;
    float increment = 0.05f;
    Renderer renderer;

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        r += increment;
        if (r > 1.f)
        {
            r = 0.f;
        }

        renderer.Draw(va, ib, shader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLint maxTextureUnits = 0;

    // 查询片段着色器可用纹理单元
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Fragment shader max texture units: " << maxTextureUnits << std::endl;

    // 查询顶点着色器可用纹理单元
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Vertex shader max texture units: " << maxTextureUnits << std::endl;

    // 查询几何着色器可用纹理单元（如果使用几何着色器）
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Geometry shader max texture units: " << maxTextureUnits << std::endl;

    GLint maxCombinedTextureUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits);
    std::cout << "Max combined texture units: " << maxCombinedTextureUnits << std::endl;

    // 销毁OpenGL上下文
    glfwTerminate();
    return 0;
}
