#pragma once
//ʱ��
namespace Hazel {
    class Timestep {
    public:
        Timestep(float time = 0.0f)
            :m_Time(time) {//m_Time��ʼΪ0

        }
        operator float() const { return m_Time; }//��������ת��
        float GetSeconds() const { return m_Time; }//��ȡʱ��
        float GetMilliseconds() const { return m_Time * 1000.0f; }//��ȡ����
    private:
        float m_Time;//ʱ��
    };
}