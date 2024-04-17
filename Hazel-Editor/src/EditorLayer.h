#pragma once
//EditorLayer²ã
#include"Hazel.h"
namespace Hazel {
    class EditorLayer :public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;
        virtual void OnAttach() override;//³õÊ¼»¯
        virtual void OnDetach() override;//Ïú»Ù
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

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
    };

}
