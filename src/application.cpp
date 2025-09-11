#include <Windows.h> // 锁定英文输入法使用
#include <imm.h>

#include <array>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> // 获取原生窗口句柄
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "camera.h"
#include "fbo.h"

#include "tests/test.h"
#include "tests/test_batch_rendering.h"
#include "tests/test_clear_color.h"
#include "tests/test_draw_cube.h"
#include "tests/test_texture_2d.h"
#include "tests/test_material.h"
#include "tests/test_lighting_maps.h"

// 让窗口宽高可以在别的翻译单元访问
extern const int WindowWidth;
extern const int WindowHeight;
const int WindowWidth  = 1920;
const int WindowHeight = 1080;

int ViewportWidth  = 1280;
int ViewportHeight = WindowHeight;

Camera MyCamera(glm::vec3(0.f, 0.f, 100.f));
static float sDeltaTime    = 0.f;
static bool bFPSModeActive = false;

// 回调函数
void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam);
void MouseMoveCallback(GLFWwindow* Window, double XPos, double YPos);

void ProcessInputFPSMode(GLFWwindow* window);

void DrawViewportWindow(const FrameBuffer& FrameBuffer,GLFWwindow* Window, ImGuiIO& io);
void DrawParametersWindow(test::Test*& CurrentTest, test::TestMenu* TestMenu);

/**
 * @brief 鼠标滚轮回调
 *
 * @param window
 * @param xoffset 水平方向滚轮滚动值，向右滚为正，向左滚为负，一般滚一下大小为1
 * @param yoffset 竖直方向滚轮滚动值，向上滚为正，向下滚为负，一般滚一下大小为1
 */
void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset);

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // 开启opengl核心模式, 移除固定管线和API
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "OpenGL实验场", NULL, NULL);
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

    std::cout << glGetString(GL_VERSION) << std::endl;

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
    glDebugMessageCallback(OpenGLDebugCallback, nullptr);

    // 窗口大小改变时的回调函数
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    ImGui::CreateContext();
    ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    float main_scale  = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling,
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 加载字体
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font =
            io.Fonts->AddFontFromFileTTF("resources/common/msyh.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    IM_ASSERT(font != nullptr);

    test::Test* currentTest  = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);

    // 指定默认的test为menu，每次从menu启动
    // currentTest = testMenu;
    currentTest = new test::TestBatchRendering("批量绘制");
    testMenu->RegisterTest<test::TestClearColor>("颜色清除");
    testMenu->RegisterTest<test::TestTexture2D>("2D纹理");
    testMenu->RegisterTest<test::TestBatchRendering>("批量绘制");
    testMenu->RegisterTest<test::TestDrawCube>("立方体");
    testMenu->RegisterTest<test::TestMaterial>("材质");
    testMenu->RegisterTest<test::TestLightingMaps>("材质贴图");

    float lastFrame = 0.f;
    FrameBuffer fbo(ViewportWidth, ViewportHeight);

    while (!glfwWindowShouldClose(window))
    {
        // 计算deltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        sDeltaTime         = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        // 处理输入
        glfwPollEvents();

        // ======================OpenGL场景绘制====================
        fbo.Bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 必须有这一行，否则OpenGL还是以为把NDC坐标转换到外层窗口坐标
        glViewport(0.f, 0.f, ViewportWidth, ViewportHeight);
        if (currentTest)
        {
            currentTest->OnUpdate(sDeltaTime);
            currentTest->OnRender();
        }
        fbo.Unbind();

        // ======================ImGui绘制====================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawViewportWindow(fbo, window, io);
        DrawParametersWindow(currentTest, testMenu);

        ImGui::Render();                                        // ImGui准备DrawData
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // ImGui执行绘制

        // 交换缓冲区显示ImGui和我们自己的绘制内容
        glfwSwapBuffers(window);
    }

    delete currentTest;

    if (currentTest != testMenu)
    {
        delete testMenu;
    }

    // ImGui清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 销毁OpenGL上下文
    glfwTerminate();
    return 0;
}

// debug回调函数
void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam)
{
    std::string sourceStr;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            sourceStr = "Other";
            break;
    }

    std::string typeStr;
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "Deprecated";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "Undefined";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            typeStr = "Other";
            break;
    }

    std::string severityStr;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            severityStr = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityStr = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severityStr = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severityStr = "NOTIFICATION";
            break;
    }

    std::cout << "[OpenGL调试信息]"
              << " Source: " << sourceStr << " | Type: " << typeStr << " | ID: " << id << " | Severity: " << severityStr
              << "\n"
              << message << "\n"
              << std::endl;
}

void ProcessInputFPSMode(GLFWwindow* window)
{
    // 非FPS相机模式直接返回
    if (!bFPSModeActive)
    {
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        MyCamera.ProcessKeyboard(ECameraMoveDirection::Forward, sDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        MyCamera.ProcessKeyboard(ECameraMoveDirection::Backward, sDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        MyCamera.ProcessKeyboard(ECameraMoveDirection::Left, sDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        MyCamera.ProcessKeyboard(ECameraMoveDirection::Right, sDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        MyCamera.ProcessKeyboard(ECameraMoveDirection::Down, sDeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E))
    {
        MyCamera.ProcessKeyboard(ECameraMoveDirection::Up, sDeltaTime);
    }
}

/**
 * @brief 鼠标移动时的回调函数。注意glfw窗口左上角坐标为(0, 0)
 *
 * @param window 鼠标所在的窗口
 * @param xpos 鼠标在窗口中的x坐标
 * @param ypos 鼠标在窗口中的y坐标
 */
void MouseMoveCallback(GLFWwindow* Window, double XPos, double YPos)
{
    static float sLastX   = 0.f;
    static float sLastY   = 0.f;
    static bool firstInit = true;

    // 非FPS相机模式直接返回
    if (!bFPSModeActive)
    {
        // 已经退出了需要保证下次进入时仍然会进行鼠标位置初始化
        firstInit = true;
        return;
    }

    // 如果sLastX和sLastY直接设置为屏幕中心，如果鼠标点击位置距viewport中心较远，第一次执行MouseMoveCallback可能会向相机传入很大的偏移量
    // 导致镜头抖动，我们在第一次调用时初始化为鼠标点击的位置就不会产生大幅度抖动了
    if (firstInit)
    {
        sLastX    = XPos;
        sLastY    = YPos;
        firstInit = false;
    }

    float xOffset = XPos - sLastX;
    float yOffset = sLastY - YPos;

    sLastX = XPos;
    sLastY = YPos;

    MyCamera.ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    MyCamera.ProcessMouseScroll(YOffset);
}

void DrawViewportWindow(const FrameBuffer& FrameBuffer, GLFWwindow* Window, ImGuiIO& io)
{
    // 绘制viewport窗口
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // 设置窗口无padding，否则会出现竖直滚动条
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f)); // pivot默认(0, 0)表示pos是左上角，(1, 1)表示pos是右下角
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(ViewportWidth), static_cast<float>(ViewportHeight)));
    ImGui::Begin("ViewPort", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    // ImGui左上角是(0, 0)，按顺时针(0, 1, 2)，(0, 2, 3)绘制两个三角形表示一个image，两个三角形的纹理从指定纹理ID中取
    // Image中的uv0用于指定左上角的纹理坐标，uv1用于指定右下角的纹理坐标
    // OpenGL纹理左下角是(0, 0)，直接传入OpenGL纹理，会导致图片上下颠倒
    // 因此我们需要把OpenGL中纹理左上角和右下角的坐标给ImGui
    ImVec2 uv0(0.f, 1.f);
    ImVec2 uv1(1.f, 0.f);
    // 必须保证Image的大小和frame buffer object一致，否则会发生拉伸，导致物体变形
    ImVec2 textureSize(FrameBuffer.GetWidth(), FrameBuffer.GetHeight());
    ImGui::Image((ImTextureID) (intptr_t) FrameBuffer.GetTexColorBufferID(), textureSize, uv0, uv1);
    // 如果image被鼠标点击了
    if (ImGui::IsItemClicked(0))
    {
        std::cout << "进入FPS模式" << std::endl;
        bFPSModeActive = true;
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (bFPSModeActive && ImGui::IsMouseReleased(0))
    {
        std::cout << "退出FPS模式" << std::endl;
        bFPSModeActive = false;
        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    ProcessInputFPSMode(Window);

    ImGui::End();
    ImGui::PopStyleVar();
}

void DrawParametersWindow(test::Test*& CurrentTest, test::TestMenu* TestMenu)
{
    if (CurrentTest)
    {
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(ViewportWidth),
                                       0.f)); // pivot默认(0, 0)表示pos是左上角，(1, 1)表示pos是右下角
        ImGui::SetNextWindowSize(
                ImVec2(static_cast<float>(WindowWidth - ViewportWidth), static_cast<float>(WindowHeight)));
        std::string testName = CurrentTest->GetDisplayName();
        ImGui::Begin(testName.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        CurrentTest->OnImGuiRender();
        if (CurrentTest != TestMenu)
        {
            ImGui::Separator();
            if (ImGui::Button("返回"))
            {
                delete CurrentTest;
                CurrentTest = TestMenu;
            }
        }

        ImGui::End();
    }
}
