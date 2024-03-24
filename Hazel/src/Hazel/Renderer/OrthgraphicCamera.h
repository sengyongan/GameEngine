#pragma once
//���������
#include<glm/glm.hpp>
namespace Hazel {
    class OrthgraphicCamera {
    public:
        OrthgraphicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);//��������ͶӰ���󣨹�������)

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position;RecalculateViewMatrix(); }

        float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation) { m_Rotation = rotation;RecalculateViewMatrix(); }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjextionMatrix() const { return m_ViewProjextionMatrix; }


    private:
        void RecalculateViewMatrix();//������ͼ����λ��
    private:
        glm::mat4 m_ProjectionMatrix;//ͶӰ���󣺹淶�������
        glm::mat4 m_ViewMatrix;//��ͼ����ģ������ڹ۲��ߵ�λ��
        glm::mat4 m_ViewProjextionMatrix;//��ͼͶӰ����=ͶӰ�������ͼ�����ģ�;���

        glm::vec3 m_Position{0.0f,0.0f,0.0f};//λ��
        float m_Rotation = 0.0f;
    };
}