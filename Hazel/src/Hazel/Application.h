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

        void pushlayer(Layer* layer);//调用层的函数
        void pushOverlayer(Layer* overlay);

        inline static  Application& Get() { return  *s_Instance; }//返回Application实例
        inline Window& GetWindow() { return  *m_Window; }//返回窗口


    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;//Windowl类型的指针m_Window//实际窗口

        bool m_Running = true;//是否运行

        LayerStack    m_Layerstack;//层
    private:
        static Application* s_Instance;//成员变量——实例


	};
	//to be define in client
	Application* CreatApplication();//创建应用程序实例

}