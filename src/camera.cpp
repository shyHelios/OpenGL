#include "camera.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int ViewportWidth  = 1280;
const int ViewportHeight = 1080;

Camera::Camera(const glm::vec3& InPosition, float InFovy, float InNear, float InFar, float InMoveSpeed,
               float InMouseSensitivity) :
    Position(InPosition),
    Fovy(InFovy),
    Near(InNear),
    Far(InFar),
    MoveSpeed(InMoveSpeed),
    MouseSensitivity(InMouseSensitivity)
{
    UpdateCameraVectors();
}

Camera::~Camera() {}

glm::vec3 Camera::GetPosition() const
{
    return Position;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    // FIXME: 这里设置的aspect ratio是正确的，但是正方体绘制不正确
    float aspectRatio = static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight);
    return glm::perspective(glm::radians(Fovy), aspectRatio, Near, Far);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, sWorldUp);
}

void Camera::ProcessKeyboard(ECameraMoveDirection Direction, float DeltaTime)
{
    float speed = MoveSpeed * DeltaTime;
    switch (Direction)
    {
        case ECameraMoveDirection::Forward:
            Position += Front * speed;
            break;
        case ECameraMoveDirection::Backward:
            Position -= Front * speed;
            break;
        case ECameraMoveDirection::Left:
            Position -= Right * speed;
            break;
        case ECameraMoveDirection::Right:
            Position += Right * speed;
            break;
        case ECameraMoveDirection::Up:
            Position += Up * speed;
            break;
        case ECameraMoveDirection::Down:
            Position -= Up * speed;
            break;
        default:
            break;
    }
}

void Camera::ProcessMouseMovement(float XOffset, float YOffset)
{
    XOffset *= MouseSensitivity;
    YOffset *= MouseSensitivity;

    Yaw += XOffset;
    Pitch += YOffset;

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float YOffset)
{
    Fovy -= YOffset;
    Fovy = glm::clamp(Fovy, 1.f, 80.f);
}

void Camera::UpdateCameraVectors()
{
    // 由于我们不允许相机视角发生roll，因此只要指定相机位置和front就可以确定一个相机的姿态
    float pitchRadians = glm::radians(Pitch);
    float yawRadians   = glm::radians(Yaw);

    Front.x = std::cosf(pitchRadians) * std::cosf(yawRadians);
    Front.y = std::sinf(pitchRadians);
    Front.z = std::cosf(pitchRadians) * std::sinf(yawRadians);

    Front = glm::normalize(Front);
    Right = glm::normalize(glm::cross(Front, sWorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
