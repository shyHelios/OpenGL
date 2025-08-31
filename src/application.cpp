#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "tests/test.h"
#include "tests/test_clear_color.h"
#include "tests/test_texture_2d.h"

int window_width  = 1280;
int window_height = 720;

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

    std::ofstream log_file("opengl_log.txt", std::ios::app);
    if (!log_file.is_open())
    {
        std::cerr << "Failed to open log file!\n";
    }

    log_file << "[OpenGL Debug Message]"
             << " Source: " << sourceStr << " | Type: " << typeStr << " | ID: " << id << " | Severity: " << severityStr
             << "\n"
             << message << "\n"
             << std::endl;
}

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    // std::cout << "Frame buffer resize: ";
    // std::cout << width << " " << height << std::endl;
    window_width  = width;
    window_height = height;
    // proj          = glm::ortho(0.f, static_cast<float>(width), 0.f, static_cast<float>(height), -1.f, 1.f);
    glViewport(0, 0, width, height);
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
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
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

    // 窗口大小改变时的回调函数
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

    // 开启颜色混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    float main_scale  = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling,
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 加载字体
    ImGuiIO &io  = ImGui::GetIO();
    ImFont *font = io.Fonts->AddFontFromFileTTF("resources/msyh.ttf", 18.0f, NULL,
                                                io.Fonts->GetGlyphRangesChineseFull());
    IM_ASSERT(font != nullptr);

    test::Test *current_test  = nullptr;
    test::TestMenu *test_menu = new test::TestMenu(current_test);

    // 指定默认的test为menu，每次从menu启动
    current_test = test_menu;

    test_menu->RegisterTest<test::TestClearColor>("TestClearColor");
    test_menu->RegisterTest<test::TestTexture2D>("TestTexture2D");

    while (!glfwWindowShouldClose(window))
    {
        // glfw处理输入
        glfwPollEvents();

        // OpenGL清屏
        glClear(GL_COLOR_BUFFER_BIT);

        // ImGui渲染开始
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (current_test)
        {
            current_test->OnUpdate(0.f);
            current_test->OnRender();
            ImGui::Begin("Test");
            if (current_test != test_menu && ImGui::Button("return"))
            {
                delete current_test;
                current_test = test_menu;
            }
            current_test->OnImGuiRender();
            ImGui::End();
        }

        ImGui::Render();                                        // ImGui准备DrawData
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // ImGui执行绘制

        // 交换缓冲区显示ImGui和我们自己的绘制内容
        glfwSwapBuffers(window);
    }

    delete current_test;

    if (current_test != test_menu)
    {
        delete test_menu;
    }

    // ImGui清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 销毁OpenGL上下文
    glfwTerminate();
    return 0;
}
