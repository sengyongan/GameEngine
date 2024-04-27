#pragma once
//EditorLayer应用层
#include"Hazel.h"
#include"Panels/SceneHierarchyPanel.h"
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
        OrthographicCameraController m_CameraController;
        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;
        Ref<Texture2D>m_Texture;
        //
        Ref<Scene> m_ActiveScene;//包含实体的场景
        Entity m_SquareEntity;//绿色实体
        Entity m_CameraEntity;
        Entity m_SenondCameraEntity;

        bool m_PrimaryCamera = true;
        //
        bool m_ViewportFocused = false, m_ViewportHovered = false;//焦点（窗口正在接收用户的输入） / 悬停（指针是否在窗口上悬停）

        glm::vec2 m_ViewportSize = {0.0f, 0.0f};

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        SceneHierarchyPanel m_SceneHierarchyPanel;//层级面板
    };

}
