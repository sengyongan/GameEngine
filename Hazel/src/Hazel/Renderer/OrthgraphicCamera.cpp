#include"hzpch.h"
#include "OrthgraphicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Hazel {//����ͶӰ����,��ͬ��͸��ͶӰ��������Ϊ����ı仯�������Ӿ��ϵ�Զ��Ч�������Ǳ�������Ĵ�С����
    OrthgraphicCamera::OrthgraphicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left,right,bottom,top,-1.0f,1.0f)),m_ViewMatrix(1.0f)
    {
        m_ViewProjextionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthgraphicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjextionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthgraphicCamera::RecalculateViewMatrix()
    {//glm::mat4(1.0f)����һ��4x4�ĵ�λ����
        glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), m_Position) * 
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));//��z����ת

        m_ViewMatrix = glm::inverse(transfrom);//����󡪡��ƶ������ʵ���ƶ�����ģ��
        m_ViewProjextionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
