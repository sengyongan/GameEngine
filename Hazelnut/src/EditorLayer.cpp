//���߲�ͻ��ˣ��༭��
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
        //m_CameraController.SetZoomLevel(5.0f);

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);
        //ECS//////////////////////////////////////////////////////////////////////////////////////
        m_ActiveScene = CreateRef<Scene>();
        auto square = m_ActiveScene->CreateEntity("Green Square");//����ʵ��
        //������
        square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f,1.0f,0.0f,1.0f});
        m_SquareEntity = square;//ȫ�ֱ���

        auto redSquare = m_ActiveScene->CreateEntity("Red Square");
        redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
        //
        m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
        m_CameraEntity.AddComponent<CameraComponent>();
        //
        m_SenondCameraEntity = m_ActiveScene->CreateEntity("Camera B");
        auto& cc = m_SenondCameraEntity.AddComponent<CameraComponent>();
        cc.Primary = false;
        //������ƽű�///////////////////////////////////////////////////
        class CameraController : public ScriptableEntity {
        public:
            void OnCreate() {
                auto& translation = GetComponent<TransformComponent>().Translation;
                translation.x = rand() % 10 - 5.0f;//�����-5����4
            }
            void OnDestory() {

            }
            void OnUpdate(Timestep ts) {
                auto& translation = GetComponent<TransformComponent>().Translation;
                float speed = 5.0f;

                if (Input::IsKeyPressed(HZ_KEY_A))
                    translation.x -= speed * ts;//x
                if (Input::IsKeyPressed(HZ_KEY_D))
                    translation.x += speed * ts;
                if (Input::IsKeyPressed(HZ_KEY_W))
                    translation.y += speed * ts;//y
                if (Input::IsKeyPressed(HZ_KEY_S))
                    translation.y -= speed * ts;
            }
        };
        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        m_SenondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        ///SceneHierarchyPanel/////////////////////////////////////////////
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();

    }

    void EditorLayer::OnUpdate(Timestep ts)
    {   //ֻ�е��״�֡ / �������ڲŻ�ִ��
        //��ȡ֡����//��ʽ��for(int i = 0;i�����ֺţ� //m_ViewportSize �� imgui������
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); //��ȡ֡������(��Ⱦ����)
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))//�ڷ���С��״̬
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//��֡���������Ŵ��ڸı�
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);//OrthographicCameraController�����Ⱦ��С�ı䣨������䣬���������Ⱦԭ����С���ᱻѹ����
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//�ó������µĳ��������camera�����Ŵ��ڸı�
        }

        //camera
        if (m_ViewportFocused) {//�����û�������
            m_CameraController.OnUpdate(ts);//�����������
        }

        // Render
        Renderer2D::ResetStats();//������
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();
        
        //scene
        m_ActiveScene->OnUpdate(ts);
        //
        m_Framebuffer->Unbind();
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
        //������ʼ�˵��������湦��
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
        //SceneHierarchyPanel//////////////////////////////////////////////////////////////////
        m_SceneHierarchyPanel.OnImGuiRender();
        /////Stats���/////////////////////////////////////////////////////////////////////////
        ImGui::Begin("Stats");

        auto statis = Renderer2D::GetStats();
        ImGui::Text("Renderer2D statis");
        ImGui::Text("Draw Calls %d", statis.DrawCalls);
        ImGui::Text("Quads Calls %d", statis.QuadCount);
        ImGui::Text("Vertices Calls %d", statis.GetTotalVertexCount());
        ImGui::Text("Indies Calls %d", statis.GetTotalIndexCount());

        ImGui::End();

        /////Viewport���/////////////////////////////////////////////////////////////////////
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });//ȡ���߿�
        ImGui::Begin("Viewport");
        //��ȡ�û��������
        m_ViewportFocused = ImGui::IsWindowFocused();//��ȡ��ǰֵ
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//����һ��������,�����Ϊ��
        //
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//��ȡ�������������
        m_ViewportSize = { (uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y };
        //���Ƶ�Image
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//��ȡ֡����ID,��Ⱦ��image
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
        ImGui::End();
        ImGui::PopStyleVar();
        ///////////////////////////
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);

    }

}