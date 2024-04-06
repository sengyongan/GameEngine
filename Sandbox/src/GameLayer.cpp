#include "GameLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Hazel;

GameLayer::GameLayer()
    : Layer("GameLayer")
{
    auto& window = Application::Get().GetWindow();
    CreateCamera(window.GetWidth(), window.GetHeight());

    Random::Init();//创建随机数生成器
}

void GameLayer::OnAttach()
{
    m_Level.Init();

    ImGuiIO io = ImGui::GetIO();
    m_Font = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Regular.ttf", 120.0f);//ImGui库创建了一个字体对象 m_Font，设置为从文件加载的，字体大小设置为120.0f
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(Hazel::Timestep ts)
{
    m_Time += ts;
    if ((int)(m_Time * 10.0f) % 8 > 4)
        m_Blink = !m_Blink;

    if (m_Level.IsGameOver())
        m_State = GameState::GameOver;

    const auto& playerPos = m_Level.GetPlayer().GetPosition();
    m_Camera->SetPosition({ playerPos.x, playerPos.y, 0.0f });

    switch (m_State)
    {
        case GameState::Play:
        {
            m_Level.OnUpdate(ts);//关卡更新-》玩家更新
            break;
        }
    }
    m_Level.GetPlayer().UpdateParticles(ts);//转移到这里一直更新，从而非开始状态，玩家没法移动，就不会产生粒子
    // Render
    Hazel::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
    Hazel::RenderCommand::Clear();

    Hazel::Renderer2D::BeginScene(*m_Camera);
    m_Level.OnRender();
    Hazel::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
    //ImGui::Begin("Settings");
    //m_Level.OnImGuiRender();
    //ImGui::End();

    // UI?

    switch (m_State)
    {
    case GameState::Play:
    {
        uint32_t playerScore = m_Level.GetPlayer().GetScore();
        std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
        ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, ImGui::GetWindowPos(), 0xffffffff, scoreStr.c_str());
        break;
    }
    case GameState::MainMenu:
    {
        auto pos = ImGui::GetWindowPos();
        auto width = Application::Get().GetWindow().GetWidth();
        auto height = Application::Get().GetWindow().GetHeight();
        pos.x += width * 0.5f - 300.0f;
        pos.y += 50.0f;
        if (m_Blink)
            ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "Click to Play!");
        break;
    }
    case GameState::GameOver:
    {
        auto pos = ImGui::GetWindowPos();
        auto width = Application::Get().GetWindow().GetWidth();
        auto height = Application::Get().GetWindow().GetHeight();
        pos.x += width * 0.5f - 300.0f;
        pos.y += 50.0f;
        if (m_Blink)
            ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "Click to Play!");

        pos.x += 200.0f;
        pos.y += 150.0f;
        uint32_t playerScore = m_Level.GetPlayer().GetScore();
        std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
        ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, pos, 0xffffffff, scoreStr.c_str());
        break;
    }
    }
}

void GameLayer::OnEvent(Hazel::Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(GameLayer::OnWindowResize));
    dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
}

bool GameLayer::OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& e)//鼠标按键回调
{
    if (m_State == GameState::GameOver)//游戏结束，点击屏幕，重新开始，重新设置游戏状态
        m_Level.Reset();

    m_State = GameState::Play; 
    return false;
}

bool GameLayer::OnWindowResize(Hazel::WindowResizeEvent& e)//回调获取当前窗口大小，摄像机大小改变
{
    CreateCamera(e.GetWidth(), e.GetHeight());
    return false;
}

void GameLayer::CreateCamera(uint32_t width, uint32_t height)//创建正交摄像机
{
    float aspectRatio = (float)width / (float)height;//16/9=1.78

    float camWidth = 8.0f;
    float bottom = -camWidth;
    float top = camWidth;
    float left = bottom * aspectRatio;
    float right = top * aspectRatio;
    m_Camera = CreateScope<OrthographicCamera>(left, right, bottom, top);
}