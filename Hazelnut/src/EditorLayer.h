#pragma once
//EditorLayer应用层
#include"Hazel.h"
#include"Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include"Hazel/Renderer/EditorCamera.h"

namespace Hazel {
    class EditorLayer :public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;
        virtual void OnAttach() override;//初始化
        virtual void OnDetach() override;//销毁
        void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender()override;
        void OnEvent(Event& event)override;

    private:
        //事件
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        //
        void OnOverlayRender();//绘制碰撞体可视化
        //
        void NewProject();
        bool OpenProject();
        void OpenProject(const std::filesystem::path& path);
        void SaveProject();
        //
        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path& path);
        void SaveScene();
        void SaveSceneAs();
        //
        void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);
        //
        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();
        void OnScenePause();
        //
        void OnDuplicateEntity();//拷贝实体
        //
        void UI_Toolbar();//工具面板
    private:
        OrthographicCameraController m_CameraController;
        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;
        Ref<Texture2D>m_Texture;
        //ecs
        Ref<Scene> m_ActiveScene;//包含实体的场景
        Ref<Scene> m_EditorScene;//当前编辑的场景
        std::filesystem::path m_EditorScenePath;//编辑场景的路径
        
        Entity m_SquareEntity;//绿色实体
        Entity m_CameraEntity;
        Entity m_SenondCameraEntity;

        Entity m_HoveredEntity;//悬停实体

        bool m_PrimaryCamera = true;

        EditorCamera m_EditorCamera;
        //
        bool m_ViewportFocused = false, m_ViewportHovered = false;//焦点（窗口正在接收用户的输入） / 悬停（指针是否在窗口上悬停）

        glm::vec2 m_ViewportSize = {0.0f, 0.0f};///视口大小
        glm::vec2 m_ViewportBounds[2];//视口边界，vec2类型数组，大小为2（左上xy，右下xy）
        //
        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
        //GizmoType
        int m_GizmoType = -1;//平移0，旋转1，缩放2
        ///是否展示物理碰撞
        bool m_ShowPhysicsColliders = false;
        //面板
        SceneHierarchyPanel m_SceneHierarchyPanel;
        Scope<ContentBrowserPanel> m_ContentBrowserPanel;
        //场景状态
        enum class SceneState {//编辑模式
            Edit = 0, Play = 1, Simulate = 2
        };
        SceneState m_SceneState = SceneState::Edit;

        // Editor physics resources开始停止图标
        Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;
    };

}
