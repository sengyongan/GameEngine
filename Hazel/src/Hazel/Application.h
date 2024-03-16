#pragma once
#include "core.h"
#include"Hazel/Events/Event.h"
#include"Window.h"
#include "Events/ApplicationEvent.h"
#include"Hazel/LayerStack.h"
#include"Hazel/imGui/imGuiLayer.h"

#include"Hazel/Renderer/Shader.h"
#include"Hazel/Renderer/Buffer.h"
#include"Hazel/Renderer/VertexArray.h"

//��Ҫ��ڵ㡪������������ʼ��
namespace Hazel {
	class HAZEL_API Application//HAZEL_APIĬ�Ͽ�
	{
	public:
		Application();
		virtual ~Application();
		void Run();//����ʵʱ���к���

        void OnEvent(Event& e);//�����¼�����

        void pushlayer(Layer* layer);//���ò�ĺ���
        void pushOverlayer(Layer* overlay);

        inline static  Application& Get() { return  *s_Instance; }//����Applicationʵ��
        inline Window& GetWindow() { return  *m_Window; }//���ش���


    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;//Windowl���͵�ָ��m_Window//ʵ�ʴ���
        imGuiLayer* m_ImGuiLayer;//imgui��
        bool m_Running = true;//�Ƿ�����
        LayerStack m_Layerstack;//��
        //
        std::shared_ptr<Shader>m_Shader;//��ɫ���ࡪ��ָ��
        std::shared_ptr<VertexArray>m_VertexArray;//����������
        //����
        std::shared_ptr<Shader>m_BlueShader;//��ɫ����
        std::shared_ptr<VertexArray>m_SquareVA;//����������
    private:
        static Application* s_Instance;//��Ա��������ʵ��
	};
	//to be define in client
	Application* CreatApplication();//����Ӧ�ó���ʵ��

}