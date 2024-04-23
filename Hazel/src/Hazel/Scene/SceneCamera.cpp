#include"hzpch.h"
#include"SceneCamera.h"
#include<glm/gtc/matrix_transform.hpp>//ortho

namespace Hazel {
    SceneCamera::SceneCamera()
    {
        RecalculateProject();
    }
    void SceneCamera::SetOrthgraphic(float size, float nearClip, float farClip)
    {
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
        RecalculateProject();
    }
    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;
        RecalculateProject();
    }
    void SceneCamera::RecalculateProject()//����ͶӰ����
    {
        float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoBottom = -m_OrthographicSize * 0.5f;
        float orthoTop = m_OrthographicSize * 0.5f;
        //�ı�ͶӰ�����������scene��update������ɫ��
        m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
    }
}