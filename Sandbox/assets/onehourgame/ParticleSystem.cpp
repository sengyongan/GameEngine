#include "ParticleSystem.h"
#include"Hazel/Renderer/Renderer2D.h"
//#include "Random.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <random>
//随机数生成器
class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());//初始化或重置随机数生成器的种子
    }

    static float Float()
    {   //得到一个在0到1之间的随机浮点数，那么用这个随机数*任何数x————范围（0——x）
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }
private:
    static std::mt19937 s_RandomEngine;//高质量的伪随机数生成器
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;//均匀整数分布对象
};
std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;
//
ParticleSystem::ParticleSystem(uint32_t maxParticle)
    :m_PoolIndex(maxParticle - 1)
{
    m_ParticlePool.resize(maxParticle);//向粒子数组添加所有粒子
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;//激活粒子
    //设置当前粒子的属性，设置为粒子属性结构体中的，添加random随机性
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
    for (auto& particle : m_ParticlePool)//循环数组中每个粒子
    {
        if (!particle.Active)//粒子未激活跳过
            continue;

        if (particle.LifeRemaining <= 0.0f)//粒子剩余生命《0
        {
            particle.Active = false;//变为未激活
            continue;
        }

        particle.LifeRemaining -= ts;//粒子剩余生命减少
        particle.Position += particle.Velocity * (float)ts;//位置更新
        particle.Rotation += 0.01f * ts;//旋转
    }
}

void ParticleSystem::OnRender(Hazel::OrthographicCamera& camera)
{
    Hazel::Renderer2D::BeginScene(camera);
    for (auto& particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        float life = particle.LifeRemaining / particle.LifeTime;//剩余生命越小，life值越小,且在0——1之间
        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);//随着生命（0——1）的变化，会在两个color之间平滑过度，
        //color.a = color.a * life;//剩余生命越小，透明度越小

        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);
        glm::vec3 position ={ particle.Position.x,particle.Position.y,0.2f };//覆盖所有
        Hazel::Renderer2D::DrawRotatedQuad(position, { size, size }, particle.Rotation, color);
    }
    Hazel::Renderer2D::EndScene();
}