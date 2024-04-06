#pragma once

#include <Hazel.h>

#include "Color.h"
#include "Random.h"

#include "ParticleSystem.h"

class Player
{
public:
    Player();

    void LoadAssets();//加载游戏资源

    void OnUpdate(Hazel::Timestep ts);
    void OnRender();

    void OnImGuiRender();

    void Reset();//重置玩家的状态
    void UpdateParticles(Hazel::Timestep ts) { m_ParticleSystem.OnUpdate(ts); }//更新玩家的粒子系统

    float GetRotation() { return m_Velocity.y * 4.0f - 90.0f; }//获取玩家的旋转角度
    const glm::vec2& GetPosition() const { return m_Position; }//获取玩家的位置

    uint32_t GetScore() const { return (uint32_t)(m_Position.x + 10.0f) / 10.0f; }//获取玩家的得分
private:
    glm::vec2 m_Position = { -10.0f, 0.0f };//玩家的位置
    glm::vec2 m_Velocity = { 5.0f, 0.0f };//玩家的速度

    float m_EnginePower = 0.5f;//引擎功率
    float m_Gravity = 0.4f;//重力加速度

    float m_Time = 0.0f;//时间
    float m_SmokeEmitInterval = 0.4f;//烟雾发射间隔
    float m_SmokeNextEmitTime = m_SmokeEmitInterval;//下一次烟雾发射的时间

    ParticleProps m_SmokeParticle, m_EngineParticle;//烟雾粒子的属性，引擎粒子的属性
    ParticleSystem m_ParticleSystem;//粒子系统

    Hazel::Ref<Hazel::Texture2D> m_ShipTexture;//飞船纹理的引用
};