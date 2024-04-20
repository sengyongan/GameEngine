#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
namespace Hazel {
    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
    {
    }
    void EditorLayer::OnAttach()
    {
        HZ_PROFILE_FUNCTION();

        m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");
        m_CameraController.SetZoomLevel(5.0f);

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();

    }

    void EditorLayer::OnUpdate(Timestep ts)
    {

        HZ_PROFILE_FUNCTION("EditorLayer::OnUpdate");

        //camera
        if (m_ViewportFocused) {//接收用户的输入
            m_CameraController.OnUpdate(ts);//可以相机控制

        }

        // Render
        Renderer2D::ResetStats();
        {
            HZ_PROFILE_SCOPE("Renderer Prep");
            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RenderCommand::Clear();
        }

        {
            static float rotation = 0.0f;
            rotation += ts * 50.0f;

            HZ_PROFILE_SCOPE("Renderer Draw");
            Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
            Hazel::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
            Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
            Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
            Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_Texture, 10.0f);
            Hazel::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_Texture, 20.0f);
            Hazel::Renderer2D::EndScene();

            Renderer2D::BeginScene(m_CameraController.GetCamera());
            for (float y = -5.0f; y < 5.0f; y += 0.5f)
            {
                for (float x = -5.0f; x < 5.0f; x += 0.5f)
                {
                    glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
                    Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
                }
            }            Renderer2D::EndScene();
            m_Framebuffer->Unbind();
        }

    }

    void EditorLayer::OnImGuiRender()
    {
        HZ_PROFILE_FUNCTION();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;//全屏
        static bool opt_padding = false;//填补
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;//标志

        // 我们使用 ImGuiWindowFlags_NoDocking 标志使父窗口不能停靠、
        // 因为如果两个停靠目标同时停靠在对方窗口中，会造成混乱。
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());//下一个窗口位置
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);//推送风格
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;//所有有一个成立，window_flags就成立
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;//等号左右需要都成立
        }

        // 当使用 ImGuiDockNodeFlags_PassthruCentralNode 时，DockSpace() 将渲染我们的背景。
        // 并处理直通孔，因此我们要求 Begin() 不渲染背景。
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // 重要：请注意，即使 Begin() 返回 false（即窗口已折叠），我们也要继续。
        // 这是因为我们要保持 DockSpace() 处于活动状态。如果 DockSpace() 处于非活动状态、
        // 所有停靠在其中的活动窗口都将失去父窗口并变成非停靠状态。
        // 我们不能保留活动窗口和非活动停靠之间的停靠关系，否则
        // 任何停靠空间/设置的改变都会导致窗口处于停滞状态，永远不可见。
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace停靠空间
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        //////////////////////////////////////////////////////////////////////////////////////
        ImGui::Begin("Setting");

        auto statis = Renderer2D::GetStats();
        ImGui::Text("Renderer2D statis");
        ImGui::Text("Draw Calls %d", statis.DrawCalls);
        ImGui::Text("Quads Calls %d", statis.QuadCount);
        ImGui::Text("Vertices Calls %d", statis.GetTotalVertexCount());
        ImGui::Text("Indies Calls %d", statis.GetTotalIndexCount());
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });//取消边框
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();//获取当前值
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//其中一个不成立,结果就为真
        //
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//获取内容区域可用性
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_ViewportSize = {(uint32_t) viewportPanelSize.x, (uint32_t)viewportPanelSize.y };
            m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);//让帧缓冲内随着窗口改变

            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);//相机渲染大小改变
        }
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//获取帧缓冲ID,渲染到image
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);

    }

}