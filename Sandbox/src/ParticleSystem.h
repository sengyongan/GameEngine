#pragma once

#include <Hazel.h>

struct ParticleProps//�������Խṹ��
{
    glm::vec2 Position;//λ��
    glm::vec2 Velocity, VelocityVariation;//�ٶ�//�ٶȵı仯��
    glm::vec4 ColorBegin, ColorEnd;//��ʼ/����ʱ����ɫ
    float SizeBegin, SizeEnd, SizeVariation;//��ʼ/����ʱ�Ĵ�С//��С�ı仯��
    float LifeTime = 1.0f;//���ӵ���������
};

class ParticleSystem//����ϵͳ��
{
public:
    ParticleSystem(uint32_t maxParticle = 1000);

    void Emit(const ParticleProps& particleProps);//��������

    void OnUpdate(Hazel::Timestep ts);//�������ӵ�״̬
    void OnRender(Hazel::OrthographicCamera& camera);//��������
private:
    struct Particle
    {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;

        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;//ʣ������

        bool Active = false;//�Ƿ񼤻�
    };
    std::vector<Particle> m_ParticlePool;
    uint32_t m_PoolIndex;//��ǰ����----����
};
