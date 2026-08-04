#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 定义移动方向的枚举，避免在代码里写死魔法数字
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// 默认相机参数
const float YAW = -90.0f; // 初始面向 -Z 轴
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // 相机属性向量
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // 欧拉角
    float Yaw;
    float Pitch;

    // 相机选项
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 构造函数（向量初始化）
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors(); // 根据初始欧拉角计算 Front, Right, Up 向量
    }

    // 核心接口 1：获取当前的 View 观察矩阵 (用于传给 Shader 的 MVP)
    glm::mat4 GetViewMatrix()
    {
        // TODO: 利用 glm::lookAt 计算并返回 View 矩阵
        // 提示：需要传入 Position, Position + Front, Up
        return glm::lookAt(Position, Position + Front, Up);
    }

    // 核心接口 2：处理键盘 WASD 输入
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        // TODO: 根据方向更新 Position 的位置
        // 提示：
        // 前进：Position += Front * velocity
        // 后退：Position -= Front * velocity
        // 左移：Position -= Right * velocity
        // 右移：Position += Right * velocity
        switch (direction)
        {
        case FORWARD:
            Position += Front * velocity;
            break;
        case BACKWARD:
            Position -= Front * velocity;
            break;
        case LEFT:
            Position -= Right * velocity;
            break;
        case RIGHT:
            Position += Right * velocity;
            break;

        }

    }

    // 核心接口 3：处理鼠标移动 (X/Y 偏移量)
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // 约束俯仰角，防止视角颠倒
        if (constrainPitch)
        {
            if (Pitch > 89.0f)  Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        // 根据更新后的 Yaw 和 Pitch 重新计算相机的方向向量
        updateCameraVectors();
    }

    // 核心接口 4：处理滚轮缩放 (Zoom)
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)   Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
    }

private:
    // 核心数学转换：从欧拉角 (Yaw, Pitch) 计算相机的 Front, Right, Up 向量
    void updateCameraVectors()
    {
        // 1. 根据 Yaw 和 Pitch 计算新的 Front 向量
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // 2. 根据 Front 和 WorldUp 计算右向量 (Right) 和上向量 (Up)
        // TODO: 补充叉乘 (glm::cross) 计算 Right 和 Up 的逻辑
        // 提示：Right = normalize(cross(Front, WorldUp))
        //      Up    = normalize(cross(Right, Front))
        Right = normalize(cross(Front, WorldUp));
        Up = normalize(cross(Right, Front));

    }
};
#pragma once
