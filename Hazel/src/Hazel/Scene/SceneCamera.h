//场景摄像机，设置投影矩阵大小
#pragma once
#include"Hazel/Renderer/Camera.h"

namespace Hazel {
    class SceneCamera : public Camera {
    public:
        //
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };//投影矩阵类型：透视 / 正交
        //
        SceneCamera();
        virtual ~SceneCamera() = default;
        //
        void SetPerspective(float verticalFOV, float nearClip, float farClip);//设置透视变量
        void SetOrthgraphic(float size, float nearClip, float farClip);//设置正交变量

        void SetViewportSize(uint32_t width, uint32_t height);//设置视口纵横比
        //Perspective
        float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
        void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
        float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
        void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }   

        //Orthographic
        float GetOrthopraghicSize() const { return m_OrthographicSize; }//获取摄像机缩放
        void SetOrthopraghicSize(float size) {m_OrthographicSize = size; RecalculateProjection();}
        float GetOrthopraghicNearClip() const { return m_OrthographicNear; }
        float GetOrthopraghicFarClip() const { return m_OrthographicFar; }
        void SetOrthopraghicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
        void SetOrthopraghicFarClip(float farClip) { m_OrthographicFar = farClip;RecalculateProjection();}

        //
        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type;RecalculateProjection();}

    private:
        void RecalculateProjection();//重新计算投影矩阵
    private:
        //
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;//值为int
        //
        float m_PerspectiveFOV = glm::radians(45.0f);//缩放
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;//远近平面

        float m_OrthographicSize = 10.0f;//缩放
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//远近平面

        float m_AspectRatio = 0.0f;//纵横比
    };
}