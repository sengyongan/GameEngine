#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
//图表
static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwwwwwdddddddwwwwwwww"
"wwwwwwwdddddddddddwDwwww"
"wwwwwwdddddddddddddwwwww"
"wwwwddddddddddddddddwwww"
"wwdddddwwwwddddddddddwww"
"wwdddddwwwwdddddddddwwww"
"wwdddddddddddddddddddwww"
"wwwwddddddddddddddddwwww"
"wwwdddddddddddddddwwwwww"
"wwwwwdddddddddddwwwwwwww"
"wwwwwwwwddddddwwwwwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
;
Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}
void Sandbox2D::OnAttach()
{
    HZ_PROFILE_FUNCTION();

    m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
    m_SpriteSheet = Hazel::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
    m_TextureStairs = Hazel::SubTexture2D::CreatFromCoords(m_SpriteSheet, { 0,11 }, { 128,128 });
    m_TextureTree = Hazel::SubTexture2D::CreatFromCoords(m_SpriteSheet, { 2,1 }, { 128,128 }, { 1,2 });

    m_MapWidth = s_MapWidth;
    m_MapHeight = strlen(s_MapTiles) / s_MapWidth;//字符总数/列数
    s_TextureMap['d'] = Hazel::SubTexture2D::CreatFromCoords(m_SpriteSheet, { 6,11 }, { 128,128 });
    s_TextureMap['w'] = Hazel::SubTexture2D::CreatFromCoords(m_SpriteSheet, { 11,11 }, { 128,128 });
    // 粒子Init here
    m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
    m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
    m_Particle.LifeTime = 1.0f;
    m_Particle.Velocity = { 0.0f, 0.0f };
    m_Particle.VelocityVariation = { 3.0f, 1.0f };
    m_Particle.Position = { 0.0f, 0.0f };

    m_CameraController.SetZoomLevel(5.0f);
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
#if 0//禁用
    {
        static float rotation = 0.0f;
        rotation += ts * 50.0f;

        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

        Hazel::Renderer2D::DrawRotatedQuad({ -1.0f,0.0f }, { 1.0f,1.0f }, glm::radians(rotation), { 0.8f,0.2f,0.3f,1.0f });
        //Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 1.0f,1.0f }, { 0.8f,0.2f,0.3f,1.0f });
        //Hazel::Renderer2D::DrawQuad({ 0.5f,0.5f }, { 0.5f,0.5f }, { 0.2f,0.2f,0.8f,1.0f });
        //Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f,-0.1f }, { 10.0f,10.0f }, m_Texture,10.0f);
        Hazel::Renderer2D::DrawRotatedQuad({ 0.0f,0.0f,-0.1f }, { 10.0f,10.0f }, glm::radians(0.0f), m_Texture, 20.0f);

        for (float y = -0.5f; y < 5.0f; y += 0.5f) {
            for (float x = -0.5f; x < 5.0f; x += 0.5f) {
                glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f,0.5f };//0.5————1.0之间的颜色
                Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f,0.45f }, color);
            }
        }
        Hazel::Renderer2D::EndScene();
    }
#endif
    //粒子
    if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_1))//按下鼠标发射粒子
    {
        auto [x, y] = Hazel::Input::GetMousePosition();
        auto width = Hazel::Application::Get().GetWindow().GetWidth();
        auto height = Hazel::Application::Get().GetWindow().GetHeight();

        auto bounds = m_CameraController.GetBounds();//获取当前边界
        auto pos = m_CameraController.GetCamera().GetPosition();
        x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
        y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
        m_Particle.Position = { x + pos.x, y + pos.y };
        for (int i = 0; i < 5; i++)
            m_ParticleSystem.Emit(m_Particle);
    }
    Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

    for (uint32_t y = 0; y < m_MapHeight; y++) {//为每个图表添加纹理
        for (uint32_t x = 0; x < m_MapWidth; x++) {
            char tiletype = s_MapTiles[x + y * m_MapWidth];//不为2为数组，连续的
            Hazel::Ref<Hazel::SubTexture2D> texture;
            if (s_TextureMap.find(tiletype) != s_TextureMap.end())
                texture = s_TextureMap[tiletype];
            else texture = m_TextureStairs;
            Hazel::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f,m_MapHeight- y - m_MapHeight / 2.0f,0.5f }, { 1.0f,1.0f }, texture);
        }
    }

    //Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f }, m_TextureStairs);
    //Hazel::Renderer2D::DrawQuad({ 1.0f,0.0f,0.0f }, { 1.0f,1.0f }, m_TextureBarrel);
    //Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f,0.0f }, { 1.0f,2.0f }, m_TextureTree);
    Hazel::Renderer2D::EndScene();

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
