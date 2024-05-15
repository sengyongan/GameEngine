#pragma once
//EditorLayerӦ�ò�
#include"Hazel.h"
#include"Panels/SceneHierarchyPanel.h"
#include"Hazel/Renderer/EditorCamera.h"

namespace Hazel {
    class EditorLayer :public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;
        virtual void OnAttach() override;//��ʼ��
        virtual void OnDetach() override;//����
        void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender()override;
        void OnEvent(Event& event)override;

    private:
        //�¼�
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
        Ref<Scene> m_ActiveScene;//����ʵ��ĳ���
        Entity m_SquareEntity;//��ɫʵ��
        Entity m_CameraEntity;
        Entity m_SenondCameraEntity;

        Entity m_HoveredEntity;//��ͣʵ��

        bool m_PrimaryCamera = true;

        EditorCamera m_EditorCamera;
        //
        bool m_ViewportFocused = false, m_ViewportHovered = false;//���㣨�������ڽ����û������룩 / ��ͣ��ָ���Ƿ��ڴ�������ͣ��

        glm::vec2 m_ViewportSize = {0.0f, 0.0f};///�ӿڴ�С
        glm::vec2 m_ViewportBounds[2];//�ӿڱ߽磬vec2�������飬��СΪ2������xy������xy��
        //
        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
        //GizmoType
        int m_GizmoType = -1;//ƽ��0����ת1������2
        ////�㼶���
        SceneHierarchyPanel m_SceneHierarchyPanel;
    };

}
