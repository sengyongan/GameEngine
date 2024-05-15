#pragma once
//EditorLayer应用层
#include"Hazel.h"
#include"Panels/SceneHierarchyPanel.h"
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
        void NewScene();
        void OpenScene();
        void SaveSceneAs();
    private:
        OrthographicCameraController m_CameraController;
        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;
        Ref<Texture2D>m_Texture;
        //ecs
        Ref<Scene> m_ActiveScene;//包含实体的场景
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
        ////层级面板
        SceneHierarchyPanel m_SceneHierarchyPanel;
    };

}
