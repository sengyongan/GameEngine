#pragma once
#include "core.h"
#include"Window.h"
#include"Hazel/Events/Event.h"
#include "Events/ApplicationEvent.h"
#include"Hazel/LayerStack.h"
#include"Hazel/imGui/imGuiLayer.h"
#include"Hazel/Core/Timestep.h"


//主要入口点——程序运行起始处
namespace Hazel {
	class HAZEL_API Application//HAZEL_API默认空
	{
	public:
		Application();
		virtual ~Application();

		void Run();//实时运行
        void OnEvent(Event& e);//循环所有事件

        void pushlayer(Layer* layer);//添加层
        void pushOverlayer(Layer* overlay);

        inline static  Application& Get() { return  *s_Instance; }//返回Application类对象
        inline Window& GetWindow() { return  *m_Window; }//返回窗口
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
    private:
        std::unique_ptr<Window> m_Window;//Window类对象

        imGuiLayer* m_ImGuiLayer;//imgui层指针

        bool m_Running = true;//是否运行
        bool m_Minimized = false;//窗口是否最小化

        LayerStack m_Layerstack;//总层类对象

        float m_LastFrameTime = 0.0f;//上一帧时间
    private:
        static Application* s_Instance;//Application类对象
	};
	
	Application* CreatApplication();//创建应用程序实例

}