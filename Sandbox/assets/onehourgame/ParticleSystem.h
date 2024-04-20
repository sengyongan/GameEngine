#pragma once

#include <Hazel.h>

struct ParticleProps//粒子属性结构体
{
    glm::vec2 Position;//位置
    glm::vec2 Velocity, VelocityVariation;//速度//速度的变化量
    glm::vec4 ColorBegin, ColorEnd;//开始/结束时的颜色
    float SizeBegin, SizeEnd, SizeVariation;//开始/结束时的大小//大小的变化量
    float LifeTime = 1.0f;//粒子的生命周期
};

class ParticleSystem//粒子系统类
{
public:
    ParticleSystem(uint32_t maxParticle = 1000);

    void Emit(const ParticleProps& particleProps);//发射粒子

    void OnUpdate(Hazel::Timestep ts);//更新粒子的状态
    void OnRender(Hazel::OrthographicCamera& camera);//绘制粒子
private:
    struct Particle
    {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;

        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;//剩余生命

        bool Active = false;//是否激活
    };
    std::vector<Particle> m_ParticlePool;
    uint32_t m_PoolIndex;//当前粒子----索引
};
