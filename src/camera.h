#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 世界空间的上方向
static const glm::vec3 sWorldUp = glm::vec3(0.f, 1.f, 0.f);

enum class ECameraMoveDirection
{
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down
};

class Camera
{
public:
    // 相机的近远平面默认分别为-0.01f和-100.f
    Camera(const glm::vec3& InPosition, float InFovy = 60.f, float InNear = 0.1f, float InFar = 1000.f,
           float InMoveSpeed = 50.f, float InMouseSensitivity = 0.1f);
    ~Camera();

    glm::vec3 GetPosition() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(ECameraMoveDirection Direction, float DeltaTime);
    void ProcessMouseMovement(float XOffset, float YOffset);
    void ProcessMouseScroll(float YOffset);

private:
    void UpdateCameraVectors();

    float Near;
    float Far;

    glm::vec3 Position;
    glm::vec3 Front; // Front向量通过Pitch和Yaw计算
    glm::vec3 Up;    // 相机的向上向量
    glm::vec3 Right; // 一般不需要明确计算，但是我们需要移动相机

    float Pitch = 0.f;
    float Yaw   = -90.f; // yaw初始化为-90度时front才为(0.f, 0.f, -1.f)

    float MoveSpeed;        // 每秒移动多少个单位
    float MouseSensitivity; // 鼠标移动导致视角变换的灵敏度
    float Fovy;             // fovy（度数）控制摄像机缩放程度
};
