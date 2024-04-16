#pragma once
//Sandbox2D²ã
#include"Hazel.h"
class Sandbox2D :public Hazel::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;
    virtual void OnAttach() override;//³õÊ¼»¯
    virtual void OnDetach() override;//Ïú»Ù
    void OnUpdate(Hazel::Timestep ts) override;
    virtual void OnImGuiRender()override;
    void OnEvent(Hazel::Event& event)override;
private:
    Hazel::OrthographicCameraController m_CameraController;
    // Temp
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;
    Hazel::Ref<Hazel::Shader> m_FlatColorShader;
    Hazel::Ref<Hazel::Texture2D>m_Texture;

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};

