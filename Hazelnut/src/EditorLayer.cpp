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
        if (m_ViewportFocused) {//�����û�������
            m_CameraController.OnUpdate(ts);//�����������

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
        static bool opt_fullscreen = true;//ȫ��
        static bool opt_padding = false;//�
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;//��־

        // ����ʹ�� ImGuiWindowFlags_NoDocking ��־ʹ�����ڲ���ͣ����
        // ��Ϊ�������ͣ��Ŀ��ͬʱͣ���ڶԷ������У�����ɻ��ҡ�
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());//��һ������λ��
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);//���ͷ��
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;//������һ��������window_flags�ͳ���
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;//�Ⱥ�������Ҫ������
        }

        // ��ʹ�� ImGuiDockNodeFlags_PassthruCentralNode ʱ��DockSpace() ����Ⱦ���ǵı�����
        // ������ֱͨ�ף��������Ҫ�� Begin() ����Ⱦ������
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // ��Ҫ����ע�⣬��ʹ Begin() ���� false�����������۵���������ҲҪ������
        // ������Ϊ����Ҫ���� DockSpace() ���ڻ״̬����� DockSpace() ���ڷǻ״̬��
        // ����ͣ�������еĻ���ڶ���ʧȥ�����ڲ���ɷ�ͣ��״̬��
        // ���ǲ��ܱ�������ںͷǻͣ��֮���ͣ����ϵ������
        // �κ�ͣ���ռ�/���õĸı䶼�ᵼ�´��ڴ���ͣ��״̬����Զ���ɼ���
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpaceͣ���ռ�
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });//ȡ���߿�
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();//��ȡ��ǰֵ
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//����һ��������,�����Ϊ��
        //
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//��ȡ�������������
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_ViewportSize = {(uint32_t) viewportPanelSize.x, (uint32_t)viewportPanelSize.y };
            m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);//��֡���������Ŵ��ڸı�

            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);//�����Ⱦ��С�ı�
        }
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//��ȡ֡����ID,��Ⱦ��image
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