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

        void pushlayer(Layer* layer);//���ò�ĺ���
        void pushOverlayer(Layer* overlay);

        inline static  Application& Get() { return  *s_Instance; }//����Applicationʵ��
        inline Window& GetWindow() { return  *m_Window; }//���ش���


    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;//Windowl���͵�ָ��m_Window//ʵ�ʴ���

        bool m_Running = true;//�Ƿ�����

        LayerStack    m_Layerstack;//��
    private:
        static Application* s_Instance;//��Ա��������ʵ��


	};
	//to be define in client
	Application* CreatApplication();//����Ӧ�ó���ʵ��

}