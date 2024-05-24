#include "EditorLayer.h"
#include <imgui/imgui.h>
//
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//
#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Utils/PlatformUtils.h"
//
#include "ImGuizmo.h"
//
#include "Hazel/Math/Math.h"

namespace Hazel {
    extern const std::filesystem::path g_AssetPath;

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
    {
    }

    void EditorLayer::OnAttach()
    {
        HZ_PROFILE_FUNCTION();
        //
        m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");
        m_IconPlay = Texture2D::Create("Resources/Icons/ContentBrowser/PlayButton.png");
        m_IconStop = Texture2D::Create("Resources/Icons/ContentBrowser/StopButton.png");
        //
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_ActiveScene = CreateRef<Scene>();

        auto commandLineArgs = Application::Get().GetCommandLineArgs();
        if (commandLineArgs.Count > 1)
        {
            auto sceneFilePath = commandLineArgs[1];
            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserialize(sceneFilePath);
        }

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

#if 0
        // Entity
        auto square = m_ActiveScene->CreateEntity("Green Square");
        square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

        auto redSquare = m_ActiveScene->CreateEntity("Red Square");
        redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

        m_SquareEntity = square;

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
        m_CameraEntity.AddComponent<CameraComponent>();

        m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
        cc.Primary = false;

        class CameraController : public ScriptableEntity
        {
        public:
            virtual void OnCreate() override
            {
                auto& translation = GetComponent<TransformComponent>().Translation;
                translation.x = rand() % 10 - 5.0f;
            }

            virtual void OnDestroy() override
            {
            }

            virtual void OnUpdate(Timestep ts) override
            {
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
        m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        HZ_PROFILE_FUNCTION();

        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        // Render
        Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        // Update scene
        switch (m_SceneState)
        {
            case SceneState::Edit://编辑模式
            {
                if (m_ViewportFocused)
                    m_CameraController.OnUpdate(ts);

                m_EditorCamera.OnUpdate(ts);//可以更新编辑器相机

                m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            }
            case SceneState::Play://播放模式仅执行的 
            {
                m_ActiveScene->OnUpdateRuntime(ts);
                break;
            }
        }
        //鼠标位置的实体id，传给着色器
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = (int)mx;//鼠标对于左下角的位置
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
        }

        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnImGuiRender()
    {
        HZ_PROFILE_FUNCTION();

        // Note: Switch this to true to enable dockspace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();
        ////Stats////////////////////////////////////////////////////////////////////////////////////////////////////////
        ImGui::Begin("Stats");

        std::string name = "None";
        if (m_HoveredEntity)
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("Hovered Entity: %s", name.c_str());

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();
        ////Viewport////////////////////////////////////////////////////////////////////////////////////////////////////////
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        if (ImGui::BeginDragDropTarget()) {//开始拖放目标
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {//有效载荷
                const wchar_t* path = (const wchar_t*)payload->Data;//获取相对目录名
                OpenScene(std::filesystem::path(g_AssetPath) / path);//完整路径
            }
            ImGui::EndDragDropTarget();
        }
        // Gizmos
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            // Camera

            // Runtime camera from entity
            // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            // const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            // const glm::mat4& cameraProjection = camera.GetProjection();
            // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

            // Editor camera
            const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            // Snap to 45 degrees for rotation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }


        ImGui::End();
        ImGui::PopStyleVar();
        //
        UI_Toolbar();
        //
        ImGui::End();
    }
    //
    void EditorLayer::UI_Toolbar()
    {
        //按钮风格
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];//悬停颜色
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];//激活颜色
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        //面板
        ImGui::Begin("##toolBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);//无装饰,无滚动条,不使用鼠标滚动
        //切换按钮图标，和点击按钮的状态
        float size = ImGui::GetWindowHeight() - 4.0f;
        Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;//当前停止图标为播放，播放时图标为停止
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));//设置为中间位置，正方形大小的一半
        if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size)))
        {   //当前点击图标，状态停止就播放，播放就停止
            if (m_SceneState == SceneState::Edit)
                OnScenePlay();//切换状态
            else if (m_SceneState == SceneState::Play)
                OnSceneStop();
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }
    //
    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
        m_EditorCamera.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        switch (e.GetKeyCode())
        {
            case Key::N:
            {
                if (control)
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
                if (control)
                {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
                }

                break;
            }
            // Scene Commands
            case Key::D:
            {
                if (control)
                    OnDuplicateEntity();

                break;
            }
            // Gizmos
            case Key::Q:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = -1;
                break;
            }
            case Key::W:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
            case Key::E:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            case Key::R:
            {
                if (!ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            }
        }
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
        }
        return false;
    }

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        m_EditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
        if (!filepath.empty())
        {
            OpenScene(filepath);
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (m_SceneState != SceneState::Edit) {//非编辑模式,就切换到编辑模式
            OnSceneStop();
        }
        if (path.extension().string() != ".hazel")
        {
            HZ_CLIENT_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        Ref<Scene> newScene = CreateRef<Scene>();//创建空场景并序列化
        SceneSerializer serializer(newScene);
        if (serializer.Deserialize(path.string()))//反序列化，文件资源选择的路径
        {
            m_EditorScene = newScene;
            m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneHierarchyPanel.SetContext(m_EditorScene);

            m_ActiveScene = m_EditorScene;
            m_EditorScenePath = path;
        }
    }
    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())//当前场景的路径，不为空
            SerializeScene(m_ActiveScene, m_EditorScenePath);//序列化场景
        else//否则需要另存场景路径
            SaveSceneAs();
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");//打开文件资源，创建/选择已有的
        if (!filepath.empty())
        {
            SerializeScene(m_ActiveScene, filepath);
            m_EditorScenePath = filepath;
        }
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)//序列化当前场景，到路径
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }
    //点击按钮时执行
    void EditorLayer::OnScenePlay() 
    {
        m_SceneState = SceneState::Play;
        m_RunTimeScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;
        m_ActiveScene->OnRuntimeStop();//
        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)//需要为编辑模式
            return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)//如果当前选择了实体
            m_EditorScene->DuplicateEntity(selectedEntity);
    }
}