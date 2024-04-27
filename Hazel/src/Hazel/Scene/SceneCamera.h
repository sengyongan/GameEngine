//���������������ͶӰ�����С
#pragma once
#include"Hazel/Renderer/Camera.h"

namespace Hazel {
    class SceneCamera : public Camera {
    public:
        //
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };//ͶӰ�������ͣ�͸�� / ����
        //
        SceneCamera();
        virtual ~SceneCamera() = default;
        //
        void SetPerspective(float verticalFOV, float nearClip, float farClip);//����͸�ӱ���
        void SetOrthgraphic(float size, float nearClip, float farClip);//������������

        void SetViewportSize(uint32_t width, uint32_t height);//�����ӿ��ݺ��
        //Perspective
        float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
        void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
        float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
        void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }   

        //Orthographic
        float GetOrthopraghicSize() const { return m_OrthographicSize; }//��ȡ���������
        void SetOrthopraghicSize(float size) {m_OrthographicSize = size; RecalculateProjection();}
        float GetOrthopraghicNearClip() const { return m_OrthographicNear; }
        float GetOrthopraghicFarClip() const { return m_OrthographicFar; }
        void SetOrthopraghicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
        void SetOrthopraghicFarClip(float farClip) { m_OrthographicFar = farClip;RecalculateProjection();}

        //
        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type;RecalculateProjection();}

    private:
        void RecalculateProjection();//���¼���ͶӰ����
    private:
        //
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;//ֵΪint
        //
        float m_PerspectiveFOV = glm::radians(45.0f);//����
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;//Զ��ƽ��

        float m_OrthographicSize = 10.0f;//����
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//Զ��ƽ��

        float m_AspectRatio = 0.0f;//�ݺ��
    };
}