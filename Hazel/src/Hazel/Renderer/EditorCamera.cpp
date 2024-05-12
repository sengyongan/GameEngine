#include "hzpch.h"
#include "EditorCamera.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseButtonCodes.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hazel {

    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
        Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))//camera类的投影矩阵，
    {
        UpdateView();
    }

    void EditorCamera::UpdateProjection()//重新更新投影矩阵
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void EditorCamera::UpdateView()//更新视口位置
    {
        // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        m_Position = CalculatePosition();//获取位置

        glm::quat orientation = GetOrientation();//获取方向
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);//四元数方向转化为矩阵
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f最大限制，视口越小，移动越慢
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }

    float EditorCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;//计算距离
        distance = std::max(distance, 0.0f);//距离最小为0
        float speed = distance * distance;//距离越大，移动越快，速度最小为0
        speed = std::min(speed, 100.0f); // 速度最大为100
        return speed;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void EditorCamera::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(Key::LeftAlt))//按住alt
        {
            //获取鼠标移动差值
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };//获取当前鼠标位置
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;//计算鼠标移动距离（求差）
            m_InitialMousePosition = mouse;//更新为本次位置
            //
            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))//鼠标中间平移
                MousePan(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))//鼠标左键旋转
                MouseRotate(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))//鼠标右键缩放
                MouseZoom(delta.y);
        }

        UpdateView();//通过更新位置和旋转变量，更新视图矩阵变量m_ViewMatrix
    }

    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::OnMouseScroll));//在事件中支持
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)//滚轮缩放
    {
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)//传入鼠标的差
    {
        auto [xSpeed, ySpeed] = PanSpeed();//获取平移速度
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;//改变焦点位置
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;//决定旋转方向
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f)
        {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    glm::vec3 EditorCamera::GetUpDirection() const//获取自身坐标的上轴
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 EditorCamera::CalculatePosition() const//由平移和缩放决定
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;//焦点
    }

    glm::quat EditorCamera::GetOrientation() const//由旋转决定
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));//转化为4元数
    }

}