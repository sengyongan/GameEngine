#include"hzpch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Hazel {//正交投影矩阵,不同于透视投影，不会因为距离的变化而产生视觉上的远近效果，而是保持物体的大小不变
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left,right,bottom,top,-1.0f,1.0f)),m_ViewMatrix(1.0f)
    {
        HZ_PROFILE_FUNCTION();

        m_ViewProjextionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        HZ_PROFILE_FUNCTION();

        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjextionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::RecalculateViewMatrix()
    {
        HZ_PROFILE_FUNCTION();

        //glm::mat4(1.0f)创建一个4x4的单位矩阵
        glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), m_Position) * 
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));//绕z轴旋转

        m_ViewMatrix = glm::inverse(transfrom);//逆矩阵——移动摄像机实则移动场景模型
        m_ViewProjextionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
