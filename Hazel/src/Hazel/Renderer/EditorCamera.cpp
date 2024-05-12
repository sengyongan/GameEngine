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
        Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))//camera���ͶӰ����
    {
        UpdateView();
    }

    void EditorCamera::UpdateProjection()//���¸���ͶӰ����
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void EditorCamera::UpdateView()//�����ӿ�λ��
    {
        // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        m_Position = CalculatePosition();//��ȡλ��

        glm::quat orientation = GetOrientation();//��ȡ����
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);//��Ԫ������ת��Ϊ����
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f������ƣ��ӿ�ԽС���ƶ�Խ��
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
        float distance = m_Distance * 0.2f;//�������
        distance = std::max(distance, 0.0f);//������СΪ0
        float speed = distance * distance;//����Խ���ƶ�Խ�죬�ٶ���СΪ0
        speed = std::min(speed, 100.0f); // �ٶ����Ϊ100
        return speed;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void EditorCamera::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(Key::LeftAlt))//��סalt
        {
            //��ȡ����ƶ���ֵ
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };//��ȡ��ǰ���λ��
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;//��������ƶ����루��
            m_InitialMousePosition = mouse;//����Ϊ����λ��
            //
            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))//����м�ƽ��
                MousePan(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))//��������ת
                MouseRotate(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))//����Ҽ�����
                MouseZoom(delta.y);
        }

        UpdateView();//ͨ������λ�ú���ת������������ͼ�������m_ViewMatrix
    }

    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::OnMouseScroll));//���¼���֧��
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)//��������
    {
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)//�������Ĳ�
    {
        auto [xSpeed, ySpeed] = PanSpeed();//��ȡƽ���ٶ�
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;//�ı佹��λ��
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;//������ת����
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
    glm::vec3 EditorCamera::GetUpDirection() const//��ȡ�������������
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

    glm::vec3 EditorCamera::CalculatePosition() const//��ƽ�ƺ����ž���
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;//����
    }

    glm::quat EditorCamera::GetOrientation() const//����ת����
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));//ת��Ϊ4Ԫ��
    }

}