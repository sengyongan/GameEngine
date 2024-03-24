#pragma once
//正交摄像机
#include<glm/glm.hpp>
namespace Hazel {
    class OrthgraphicCamera {
    public:
        OrthgraphicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);//重新设置投影矩阵（滚轮缩放)

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position;RecalculateViewMatrix(); }

        float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation) { m_Rotation = rotation;RecalculateViewMatrix(); }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjextionMatrix() const { return m_ViewProjextionMatrix; }


    private:
        void RecalculateViewMatrix();//计算视图矩阵位置
    private:
        glm::mat4 m_ProjectionMatrix;//投影矩阵：规范到摄像机
        glm::mat4 m_ViewMatrix;//视图矩阵：模型相对于观察者的位置
        glm::mat4 m_ViewProjextionMatrix;//视图投影矩阵=投影矩阵×视图矩阵×模型矩阵

        glm::vec3 m_Position{0.0f,0.0f,0.0f};//位置
        float m_Rotation = 0.0f;
    };
}