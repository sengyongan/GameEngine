#pragma once

#include <Hazel.h>

#include "Color.h"
#include "Random.h"

#include "ParticleSystem.h"

class Player
{
public:
    Player();

    void LoadAssets();//������Ϸ��Դ

    void OnUpdate(Hazel::Timestep ts);
    void OnRender();

    void OnImGuiRender();

    void Reset();//������ҵ�״̬
    void UpdateParticles(Hazel::Timestep ts) { m_ParticleSystem.OnUpdate(ts); }//������ҵ�����ϵͳ

    float GetRotation() { return m_Velocity.y * 4.0f - 90.0f; }//��ȡ��ҵ���ת�Ƕ�
    const glm::vec2& GetPosition() const { return m_Position; }//��ȡ��ҵ�λ��

    uint32_t GetScore() const { return (uint32_t)(m_Position.x + 10.0f) / 10.0f; }//��ȡ��ҵĵ÷�
private:
    glm::vec2 m_Position = { -10.0f, 0.0f };//��ҵ�λ��
    glm::vec2 m_Velocity = { 5.0f, 0.0f };//��ҵ��ٶ�

    float m_EnginePower = 0.5f;//���湦��
    float m_Gravity = 0.4f;//�������ٶ�

    float m_Time = 0.0f;//ʱ��
    float m_SmokeEmitInterval = 0.4f;//��������
    float m_SmokeNextEmitTime = m_SmokeEmitInterval;//��һ���������ʱ��

    ParticleProps m_SmokeParticle, m_EngineParticle;//�������ӵ����ԣ��������ӵ�����
    ParticleSystem m_ParticleSystem;//����ϵͳ

    Hazel::Ref<Hazel::Texture2D> m_ShipTexture;//�ɴ����������
};