#pragma once
#include "core.h"
#include"Hazel/Events/Event.h"
#include"Window.h"
#include "Events/ApplicationEvent.h"

#include"Hazel/LayerStack.h"

//��Ҫ��ڵ㡪������������ʼ��
namespace Hazel {
	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();//����ʵʱ���к���

        void OnEvent(Event& e);//�����¼�����

        void pushlayer(Layer* layer);
        void pushOverlayer(Layer* layer);

    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;//Windowl���͵�ָ��m_Window//ʵ�ʴ���

        bool m_Running = true;//bool

        LayerStack    m_Layerstack;


	};
	//to be define in client
	Application* CreatApplication();//����Ӧ�ó���ʵ��

}