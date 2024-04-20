#pragma once
//camera controller�������
#include"Hazel/Renderer/OrthographicCamera.h"
#include"Hazel/Core/Timestep.h"

#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Events/MouseEvent.h"
namespace Hazel {
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);//��߱�, �Ƿ�������ת

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

        void OnResize(float Width, float Height);

        OrthographicCamera& GetCamera() { return m_Camera; }
        const OrthographicCamera& GetCamera()const { return m_Camera; }

        float GetZoomLevel()const { return m_ZoomLevel; }
        void SetZoomLevel(float level) {
            m_ZoomLevel = level;
        }

    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);//�������¼��������ų���
        bool OnWindowResized(WindowResizeEvent& e);//���ڵ�����С���������ͶӰ��ͼ����Ϊ���ڿ�߱�
    private:
        float m_AspectRatio;//��߱�
        float m_ZoomLevel = 1.0f;//���ż���

        OrthographicCamera m_Camera;//��ȡ�����


        bool m_Rotation;//�Ƿ�������ת
        glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
        float m_CameraRotation = 0.0f;
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };

}
