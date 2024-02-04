#pragma once
#include "core.h"
#include"Hazel/Events/Event.h"
#include"Window.h"
#include "Events/ApplicationEvent.h"

#include"Hazel/LayerStack.h"

//主要入口点——程序运行起始处
namespace Hazel {
	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();//声明实时运行函数

        void OnEvent(Event& e);//接受事件类型

        void pushlayer(Layer* layer);
        void pushOverlayer(Layer* layer);

    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;//Windowl类型的指针m_Window//实际窗口

        bool m_Running = true;//bool

        LayerStack    m_Layerstack;


	};
	//to be define in client
	Application* CreatApplication();//创建应用程序实例

}