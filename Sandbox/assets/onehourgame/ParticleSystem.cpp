#include "ParticleSystem.h"
#include"Hazel/Renderer/Renderer2D.h"
//#include "Random.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <random>
//�����������
class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());//��ʼ�������������������������
    }

    static float Float()
    {   //�õ�һ����0��1֮����������������ô����������*�κ���x����������Χ��0����x��
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }
private:
    static std::mt19937 s_RandomEngine;//��������α�����������
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;//���������ֲ�����
};
std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;
//
ParticleSystem::ParticleSystem(uint32_t maxParticle)
    :m_PoolIndex(maxParticle - 1)
{
    m_ParticlePool.resize(maxParticle);//���������������������
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;//��������
    //���õ�ǰ���ӵ����ԣ�����Ϊ�������Խṹ���еģ����random�����
    particle.Position = particleProps.Position;
    particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

    // Velocity
    particle.Velocity = particleProps.Velocity;
    particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
    particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

    // Color
    particle.ColorBegin = particleProps.ColorBegin;
    particle.ColorEnd = particleProps.ColorEnd;

    // Size
    particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
    particle.SizeEnd = particleProps.SizeEnd;

    // Life
    particle.LifeTime = particleProps.LifeTime;
    particle.LifeRemaining = particleProps.LifeTime;
    //
    m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

void ParticleSystem::OnUpdate(Hazel::Timestep ts)
{
    for (auto& particle : m_ParticlePool)//ѭ��������ÿ������
    {
        if (!particle.Active)//����δ��������
            continue;

        if (particle.LifeRemaining <= 0.0f)//����ʣ��������0
        {
            particle.Active = false;//��Ϊδ����
            continue;
        }

        particle.LifeRemaining -= ts;//����ʣ����������
        particle.Position += particle.Velocity * (float)ts;//λ�ø���
        particle.Rotation += 0.01f * ts;//��ת
    }
}

void ParticleSystem::OnRender(Hazel::OrthographicCamera& camera)
{
    Hazel::Renderer2D::BeginScene(camera);
    for (auto& particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        float life = particle.LifeRemaining / particle.LifeTime;//ʣ������ԽС��lifeֵԽС,����0����1֮��
        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);//����������0����1���ı仯����������color֮��ƽ�����ȣ�
        //color.a = color.a * life;//ʣ������ԽС��͸����ԽС

        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);
        glm::vec3 position ={ particle.Position.x,particle.Position.y,0.2f };//��������
        Hazel::Renderer2D::DrawRotatedQuad(position, { size, size }, particle.Rotation, color);
    }
    Hazel::Renderer2D::EndScene();
}