#pragma once
//Sandbox2D层
#include"Hazel.h"
#include"ParticleSystem.h"
class Sandbox2D :public Hazel::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;
    virtual void OnAttach() override;//初始化
    virtual void OnDetach() override;//销毁
    void OnUpdate(Hazel::Timestep ts) override;
    virtual void OnImGuiRender()override;
    void OnEvent(Hazel::Event& event)override;
private:
    Hazel::OrthographicCameraController m_CameraController;
    // Temp
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;
    Hazel::Ref<Hazel::Shader> m_FlatColorShader;
    Hazel::Ref<Hazel::Texture2D>m_Texture;
    Hazel::Ref<Hazel::Texture2D>m_SpriteSheet;
    Hazel::Ref<Hazel::SubTexture2D>m_TextureStairs,m_TextureBarrel,m_TextureTree;//从纹理获取的子纹理

    struct ProfileResult
    {
        const char* Name;
        float Time;
    };

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

    ParticleSystem m_ParticleSystem;//粒子系统
    ParticleProps m_Particle;//粒子属性

    uint32_t m_MapWidth, m_MapHeight;
    std::unordered_map<char, Hazel::Ref<Hazel::SubTexture2D>> s_TextureMap;//图块（哈希表）（字符，子纹理）
};

