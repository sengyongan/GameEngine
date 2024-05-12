//���߲�ͻ��ˣ��༭��
#include "EditorLayer.h"
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Scene/SceneSerializer.h"
#include"Hazel/Utils/PlatformUtils.h"

#include"ImGuizmo.h"

#include"Hazel/Math/Math.h"

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

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
#if 0 
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

                if (Input::IsKeyPressed(Key::A))
                    translation.x -= speed * ts;
                if (Input::IsKeyPressed(Key::D))
                    translation.x += speed * ts;
                if (Input::IsKeyPressed(Key::W))
                    translation.y += speed * ts;
                if (Input::IsKeyPressed(Key::S))
                    translation.y -= speed * ts;
            }
        };
        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        m_SenondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
        ///SceneHierarchyPanel/////////////////////////////////////////////
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        //
        //SceneSerializer serializer(m_ActiveScene);
        //serializer.Serialize("assets/scenes/Example.hazel");
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
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

        //camera
        if (m_ViewportFocused) {//�����û�������
            m_CameraController.OnUpdate(ts);//�����������
        }
        m_EditorCamera.OnUpdate(ts);
        // Render
        Renderer2D::ResetStats();//������
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();
        
        //scene
        m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
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

        ///DockSpaceͣ���ռ�////////////////////////////////////////////////////////////////////////////////////////////////////
        ImGuiIO& io = ImGui::GetIO();
        //����ͣ����С�ߴ�
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        //
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;
        ///Line()////////////////////////////////////////////////////////////////////////////
        //������ʼ�˵��������湦��
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Line"))
            {
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                //ImGui::MenuItem("Padding", NULL, &opt_padding);
                //ImGui::Separator();
                if (ImGui::MenuItem("New...", "Ctrl+N")){//������ļ����µĳ�����
                    NewScene();
                }

                if (ImGui::MenuItem("Open...","Ctrl+O"))//���ļ�
                {
                    OpenScene();
                }

                if (ImGui::MenuItem("Save As...","Ctrl+Shift+S"))//���Ϊ�ļ�
                {
                    SaveSceneAs();
                }

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

        /////Viewport���/////////////////////////////////////////////////////////////////////////////////////
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
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }); 
        ////////////////////////
        //Gizmos/////////////////////
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();//��ǰѡ�е�ʵ��

        if (selectedEntity && m_GizmoType != -1) {//ѡ����ĳ���仯�Ž��л���
            ImGuizmo::SetOrthographic(false);//֧��͸��
            ImGuizmo::SetDrawlist();
            //
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Camera
            //auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();//�������
            //const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            //const glm::mat4& cameraProjection = camera.GetProjection();//��ȡͶӰ����
            //glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());//��ȡ���������λ�õ�
            //
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();//��ȡͶӰ����
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();//��ȡ���������λ�õ�

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();//ʵ���λ��

            // Snapping��׽
            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            // Snap to 45 degrees for rotation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };

            //Gizmos/////
            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);
            //����
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;//������ת��
                tc.Translation = translation;
                tc.Rotation += deltaRotation;//�������������
                tc.Scale = scale;
            }
        }
        //////////
        ImGui::End();
        ImGui::PopStyleVar();
        ///////////////////////////
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);
        m_EditorCamera.OnEvent(event);
        //
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)//����KeyPressedEvent
    {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;
        //ctrl / shift(�������ң�
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode())
        {
            case Key::N://����n
            {
                if (control)//����ctrl
                    NewScene();

                break;
            }
            case Key::O:
            {
                if (control)
                    OpenScene();

                break;
            }
            case Key::S:
            {
                if (control && shift)
                    SaveSceneAs();

                break;
            }
            // Gizmos
            case Key::Q:
                m_GizmoType = -1;
                break;
            case Key::W:
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case Key::E:
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            case Key::R:
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
        }
    }

    void EditorLayer::NewScene()
    {   //�µĿճ���
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");//���Ҿ���.hazel��չ���������ļ�
        if (!filepath.empty())//�ҵ���
        {
            m_ActiveScene = CreateRef<Scene>();
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//�����ӿڴ�С
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);//�������

            SceneSerializer serializer(m_ActiveScene);//�����л�����
            serializer.Deserialize(filepath);
        }
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
        if (!filepath.empty())
        {
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(filepath);
        }
    }

}