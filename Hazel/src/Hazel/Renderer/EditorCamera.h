#pragma once
//编辑器摄像机
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
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);//透视摄像机
        //
        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);
        //位置
        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }
        //设置视口大小
        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
        //视图矩阵
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }//获取视图投影矩阵
        //获取
        glm::vec3 GetUpDirection() const;//获取上方向
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetPosition() const { return m_Position; }//获取位置
        glm::quat GetOrientation() const;//获取方向
        //获取轴
        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }
    private:
        //更新 投影，视图矩阵m_Projection和m_ViewMatrix变量
        void UpdateProjection();
        void UpdateView();//更新视口位置（由平移和旋转和缩放）决定
        //鼠标控制
        bool OnMouseScroll(MouseScrolledEvent& e);
        //鼠标平移，旋转，缩放
        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);
        //计算位置
        glm::vec3 CalculatePosition() const;
        //鼠标平移，旋转，缩放的速度
        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;
    private:
        //初始投影矩阵数据
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
        ////视图矩阵（投影矩阵在camera类中//////////////////////////
        glm::mat4 m_ViewMatrix;
        /////////////////////////////////////
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };//当次目标点
        //初始鼠标位置
        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
        //
        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;//仰角和偏航角
        //视口大小
        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };

}