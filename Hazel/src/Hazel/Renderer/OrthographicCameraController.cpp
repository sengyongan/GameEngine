#include "hzpch.h"
#include "OrthographicCameraController.h"
#include"Hazel/Core/Input.h"
#include"Hazel/Core/KeyCodes.h"
namespace Hazel {//float left, float right, float bottom, float top当进行m_ZoomLevel缩放，会4个参数都改变，例如16 / 9 = 1.78{-1.78，1.78，-1，1}
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        :m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_AspectRatio(aspectRatio), m_Rotation(rotation)
    {
    }
    void OrthographicCameraController::OnUpdate(Timestep ts)
    {
        HZ_PROFILE_FUNCTION();

        //HZ_CLIENT_TRACE("time:{0}s", ts.GetSeconds());获取时差
        //float ts = ts;隐式强制转换，发生返回m_Time
        if (Input::IsKeyPressed(Key::A))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        else if (Input::IsKeyPressed(Key::D))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }

        if (Input::IsKeyPressed(Key::W))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        else if (Input::IsKeyPressed(Key::S))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }

        if (m_Rotation)
        {
            if (Input::IsKeyPressed(Key::Q))
                m_CameraRotation += m_CameraRotationSpeed * ts;
            if (Input::IsKeyPressed(Key::E))
                m_CameraRotation -= m_CameraRotationSpeed * ts;

            if (m_CameraRotation > 180.0f)
                m_CameraRotation -= 360.0f;
            else if (m_CameraRotation <= -180.0f)
                m_CameraRotation += 360.0f;

            m_Camera.SetRotation(m_CameraRotation);
        }
        m_Camera.SetPosition(m_CameraPosition);
        //
        m_CameraTranslationSpeed = m_ZoomLevel;//m_ZoomLevel越小，窗口越小，物体越大，距离越近，移动越慢
    }
    void OrthographicCameraController::OnEvent(Event& e)
    {
        HZ_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);//<MouseScrolledEvent>显示模板实参
        dispatcher.Dispatch <MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch <WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
    }
    void OrthographicCameraController::OnResize(float width, float height)
    {
        HZ_PROFILE_FUNCTION();

        m_AspectRatio = width / height;
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    }
    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        HZ_PROFILE_FUNCTION();
        //滚轮向上offset增加，m_ZoomLevel减小，视口变小，物体占比大
        m_ZoomLevel -= e.GetYOffset() * 0.5f;//设置纵坐标偏移为缩放级别
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);//最小为0.25（阻止越过图片)
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }
    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        HZ_PROFILE_FUNCTION();

        OnResize((float)e.GetWidth() , (float)e.GetHeight());
        return false;
    }
}