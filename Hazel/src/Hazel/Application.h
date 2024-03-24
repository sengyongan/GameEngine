#pragma once
#include "core.h"
#include"Window.h"
#include"Hazel/Events/Event.h"
#include "Events/ApplicationEvent.h"
#include"Hazel/LayerStack.h"
#include"Hazel/imGui/imGuiLayer.h"
#include"Hazel/Core/Timestep.h"


//��Ҫ��ڵ㡪������������ʼ��
namespace Hazel {
	class HAZEL_API Application//HAZEL_APIĬ�Ͽ�
	{
	public:
		Application();
		virtual ~Application();

		void Run();//ʵʱ����
        void OnEvent(Event& e);//ѭ�������¼�

        void pushlayer(Layer* layer);//��Ӳ�
        void pushOverlayer(Layer* overlay);

        inline static  Application& Get() { return  *s_Instance; }//����Application�����
        inline Window& GetWindow() { return  *m_Window; }//���ش���
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
    private:
        std::unique_ptr<Window> m_Window;//Window�����

        imGuiLayer* m_ImGuiLayer;//imgui��ָ��

        bool m_Running = true;//�Ƿ�����
        bool m_Minimized = false;//�����Ƿ���С��

        LayerStack m_Layerstack;//�ܲ������

        float m_LastFrameTime = 0.0f;//��һ֡ʱ��
    private:
        static Application* s_Instance;//Application�����
	};
	
	Application* CreatApplication();//����Ӧ�ó���ʵ��

}