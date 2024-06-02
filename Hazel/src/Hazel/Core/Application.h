#pragma once
#include"Window.h"
#include"Hazel/Events/Event.h"
#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Core/LayerStack.h"
#include"Hazel/imGui/imGuiLayer.h"
#include"Hazel/Core/Timestep.h"


//主要入口点——程序运行起始处
namespace Hazel {
    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](int index) const
        {
            HZ_CORE_ASSERT(index < Count);
            return Args[index];
        }
    };
    struct ApplicationSpecification//应用层规范
    {
        std::string Name = "Hazel Application";
        std::string WorkingDirectory;//工作目录
        ApplicationCommandLineArgs CommandLineArgs;//应用程序命令行参数 
    };

	class  Application//默认空
	{
	public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();
        const ApplicationSpecification& GetSpecification() const { return m_Specification; }

        void SubmitToMainThread(const std::function<void()>& function);// 提交到主线程
		//
        void Run();//实时运行
        void OnEvent(Event& e);//循环所有事件

        void pushlayer(Layer* layer);//添加层
        void pushOverlayer(Layer* overlay);
        void Close();

        imGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

        inline static  Application& Get() { return  *s_Instance; }//返回Application类对象
        inline Window& GetWindow() { return  *m_Window; }//返回窗口
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        void ExecuteMainThreadQueue();//执行主线程队列
    private:
        //
        ApplicationSpecification m_Specification;
        //
        std::unique_ptr<Window> m_Window;//Window类对象

        imGuiLayer* m_ImGuiLayer;//imgui层指针

        bool m_Running = true;//是否运行
        bool m_Minimized = false;//窗口是否最小化

        LayerStack m_Layerstack;//总层类对象

        float m_LastFrameTime = 0.0f;//上一帧时间

        std::vector<std::function<void()>> m_MainThreadQueue;//主线程队列《void func》
        std::mutex m_MainThreadQueueMutex;//静态主线程
    private:
        static Application* s_Instance;//Application类对象
	};
	
    Application* CreateApplication(ApplicationCommandLineArgs args);
    //创建应用程序实例

}