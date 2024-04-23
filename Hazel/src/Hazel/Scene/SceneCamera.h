//�������������imgui������λ�ã�
#pragma once
#include"Hazel/Renderer/Camera.h"

namespace Hazel {
    class SceneCamera : public Camera {
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthgraphic(float size, float nearClip, float farClip);//������������

        void SetViewportSize(uint32_t width, uint32_t height);//�����ӿ��ݺ��

        float GetOrthopraghicSize() const { return m_OrthographicSize; }//��ȡ���������
        void SetOrthopraghicSize(float size) {
            m_OrthographicSize = size; RecalculateProject();
        }
    private:
        void RecalculateProject();//���¼���ͶӰ����
    private:
        float m_OrthographicSize = 10.0f;//����
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//Զ��ƽ��

        float m_AspectRatio = 0.0f;//�ݺ��
    };
}