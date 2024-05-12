#pragma once
//�༭�������
#include "Camera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Hazel {

    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);//͸�������
        //
        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);
        //λ��
        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }
        //�����ӿڴ�С
        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
        //��ͼ����
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }//��ȡ��ͼͶӰ����
        //��ȡ
        glm::vec3 GetUpDirection() const;//��ȡ�Ϸ���
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetPosition() const { return m_Position; }//��ȡλ��
        glm::quat GetOrientation() const;//��ȡ����
        //��ȡ��
        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }
    private:
        //���� ͶӰ����ͼ����m_Projection��m_ViewMatrix����
        void UpdateProjection();
        void UpdateView();//�����ӿ�λ�ã���ƽ�ƺ���ת�����ţ�����
        //������
        bool OnMouseScroll(MouseScrolledEvent& e);
        //���ƽ�ƣ���ת������
        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);
        //����λ��
        glm::vec3 CalculatePosition() const;
        //���ƽ�ƣ���ת�����ŵ��ٶ�
        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;
    private:
        //��ʼͶӰ��������
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
        ////��ͼ����ͶӰ������camera����//////////////////////////
        glm::mat4 m_ViewMatrix;
        /////////////////////////////////////
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };//����Ŀ���
        //��ʼ���λ��
        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
        //
        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;//���Ǻ�ƫ����
        //�ӿڴ�С
        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };

}