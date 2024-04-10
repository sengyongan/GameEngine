#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}
void Sandbox2D::OnAttach()
{
    HZ_PROFILE_FUNCTION();

    m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");

    // 粒子Init here
    m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
    m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
    m_Particle.LifeTime = 1.0f;
    m_Particle.Velocity = { 0.0f, 0.0f };
    m_Particle.VelocityVariation = { 3.0f, 1.0f };
    m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
    HZ_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{

    HZ_PROFILE_FUNCTION("Sandbox2D::OnUpdate");

    //camera
        m_CameraController.OnUpdate(ts);

    // Render
        Hazel::Renderer2D::ResetStats();
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Hazel::RenderCommand::Clear();
    }

    {   
        static float rotation = 0.0f;
        rotation += ts * 50.0f;

        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

        Hazel::Renderer2D::DrawRotatedQuad({ -1.0f,0.0f }, { 1.0f,1.0f }, glm::radians(rotation),{ 0.8f,0.2f,0.3f,1.0f });
        //Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 1.0f,1.0f }, { 0.8f,0.2f,0.3f,1.0f });
        //Hazel::Renderer2D::DrawQuad({ 0.5f,0.5f }, { 0.5f,0.5f }, { 0.2f,0.2f,0.8f,1.0f });
        //Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f,-0.1f }, { 10.0f,10.0f }, m_Texture,10.0f);
        Hazel::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f,-0.1f }, { 10.0f,10.0f }, glm::radians(0.0f ), m_Texture,20.0f);

        for (float y = -0.5f; y < 5.0f; y+=0.5f) {
            for (float x = -0.5f; x < 5.0f; x+=0.5f) {
                glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f,0.5f };//0.5————1.0之间的颜色
                Hazel::Renderer2D::DrawQuad({x, y}, { 0.45f,0.45f }, color);
            }
        }
        Hazel::Renderer2D::EndScene();
    }
    //粒子
    if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_1))
    {
        auto [x, y] = Hazel::Input::GetMousePosition();
        auto width = Hazel::Application::Get().GetWindow().GetWidth();
        auto height = Hazel::Application::Get().GetWindow().GetHeight();

        auto bounds = m_CameraController.GetBounds();
        auto pos = m_CameraController.GetCamera().GetPosition();
        x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
        y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
        m_Particle.Position = { x + pos.x, y + pos.y };
        for (int i = 0; i < 5; i++)
            m_ParticleSystem.Emit(m_Particle);
    }

    m_ParticleSystem.OnUpdate(ts);
    m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
    HZ_PROFILE_FUNCTION();
    ImGui::Begin("Setting");

    auto statis = Hazel::Renderer2D::GetStats();
    ImGui::Text("Renderer2D statis");
    ImGui::Text("Draw Calls %d", statis.DrawCalls);
    ImGui::Text("Quads Calls %d", statis.QuadCount);
    ImGui::Text("Vertices Calls %d", statis.GetTotalVertexCount());
    ImGui::Text("Indies Calls %d", statis.GetTotalIndexCount());

    ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

    ImGui::End();

}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
    m_CameraController.OnEvent(event);

}
