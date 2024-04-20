#pragma once
//camera controller相机控制
#include"Hazel/Renderer/OrthographicCamera.h"
#include"Hazel/Core/Timestep.h"

#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Events/MouseEvent.h"
namespace Hazel {
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);//宽高比, 是否允许旋转

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
        bool OnMouseScrolled(MouseScrolledEvent& e);//鼠标滚动事件——缩放场景
        bool OnWindowResized(WindowResizeEvent& e);//窗口调整大小——摄像机投影视图设置为窗口宽高比
    private:
        float m_AspectRatio;//宽高比
        float m_ZoomLevel = 1.0f;//缩放级别

        OrthographicCamera m_Camera;//获取摄像机


        bool m_Rotation;//是否允许旋转
        glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
        float m_CameraRotation = 0.0f;
        float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
    };

}
