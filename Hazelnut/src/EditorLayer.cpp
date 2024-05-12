//工具层客户端：编辑器
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
        auto square = m_ActiveScene->CreateEntity("Green Square");//创建实体
        //添加组件
        square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f,1.0f,0.0f,1.0f});
        m_SquareEntity = square;//全局变量

        auto redSquare = m_ActiveScene->CreateEntity("Red Square");
        redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
        //
        m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
        m_CameraEntity.AddComponent<CameraComponent>();
        //
        m_SenondCameraEntity = m_ActiveScene->CreateEntity("Camera B");
        auto& cc = m_SenondCameraEntity.AddComponent<CameraComponent>();
        cc.Primary = false;
        //相机控制脚本///////////////////////////////////////////////////
        class CameraController : public ScriptableEntity {
        public:
            void OnCreate() {
                auto& translation = GetComponent<TransformComponent>().Translation;
                translation.x = rand() % 10 - 5.0f;//随机数-5——4
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
    {   //只有当首次帧 / 调整窗口才会执行
        //获取帧缓冲//形式：for(int i = 0;i……分号） //m_ViewportSize ， imgui的区域
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); //获取帧缓冲区(渲染区域)
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))//在非最小化状态
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//让帧缓冲内随着窗口改变
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);//OrthographicCameraController相机渲染大小改变（如果不变，相机依旧渲染原来大小，会被压缩）
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//让场景（新的场景摄像机camera）随着窗口改变
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

        //camera
        if (m_ViewportFocused) {//接收用户的输入
            m_CameraController.OnUpdate(ts);//可以相机控制
        }
        m_EditorCamera.OnUpdate(ts);
        // Render
        Renderer2D::ResetStats();//？？？
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

        ///DockSpace停靠空间////////////////////////////////////////////////////////////////////////////////////////////////////
        ImGuiIO& io = ImGui::GetIO();
        //设置停靠最小尺寸
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
        //创建开始菜单，有下面功能
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Line"))
            {
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                //ImGui::MenuItem("Padding", NULL, &opt_padding);
                //ImGui::Separator();
                if (ImGui::MenuItem("New...", "Ctrl+N")){//添加新文件（新的场景）
                    NewScene();
                }

                if (ImGui::MenuItem("Open...","Ctrl+O"))//打开文件
                {
                    OpenScene();
                }

                if (ImGui::MenuItem("Save As...","Ctrl+Shift+S"))//另存为文件
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
        /////Stats面板/////////////////////////////////////////////////////////////////////////
        ImGui::Begin("Stats");

        auto statis = Renderer2D::GetStats();
        ImGui::Text("Renderer2D statis");
        ImGui::Text("Draw Calls %d", statis.DrawCalls);
        ImGui::Text("Quads Calls %d", statis.QuadCount);
        ImGui::Text("Vertices Calls %d", statis.GetTotalVertexCount());
        ImGui::Text("Indies Calls %d", statis.GetTotalIndexCount());

        ImGui::End();

        /////Viewport面板/////////////////////////////////////////////////////////////////////////////////////
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });//取消边框
        ImGui::Begin("Viewport");
        //获取用户操作情况
        m_ViewportFocused = ImGui::IsWindowFocused();//获取当前值
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);//其中一个不成立,结果就为真
        //
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//获取内容区域可用性
        m_ViewportSize = { (uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y };
        //绘制到Image
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();//获取帧缓冲ID,渲染到image
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }); 
        ////////////////////////
        //Gizmos/////////////////////
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();//当前选中的实体

        if (selectedEntity && m_GizmoType != -1) {//选择了某个变化才进行绘制
            ImGuizmo::SetOrthographic(false);//支持透视
            ImGuizmo::SetDrawlist();
            //
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Camera
            //auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();//主摄像机
            //const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            //const glm::mat4& cameraProjection = camera.GetProjection();//获取投影矩阵
            //glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());//获取主摄像机的位置的
            //
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();//获取投影矩阵
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();//获取主摄像机的位置的

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();//实体的位置

            // Snapping捕捉
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
            //控制
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;//计算旋转差
                tc.Translation = translation;
                tc.Rotation += deltaRotation;//避免万向节死锁
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

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)//传入KeyPressedEvent
    {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;
        //ctrl / shift(键盘左右）
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode())
        {
            case Key::N://按下n
            {
                if (control)//按下ctrl
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
    {   //新的空场景
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");//查找具有.hazel扩展名的所有文件
        if (!filepath.empty())//找到了
        {
            m_ActiveScene = CreateRef<Scene>();
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);//更新视口大小
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);//场景面板

            SceneSerializer serializer(m_ActiveScene);//反序列化场景
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