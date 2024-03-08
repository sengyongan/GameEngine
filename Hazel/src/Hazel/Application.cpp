#include"hzpch.h"
#include "Application.h"
#include"Hazel/Input.h"
#include"Window.h"
#include<glad/glad.h>
namespace Hazel {
    //目前绑定OnEvent函数参数为——placeholders占位符的类模板（未知）
#define BIND_ENENT_FN(x)  std::bind(&Application::x, this, std::placeholders::_1)//绑定&Application::x事件

     Application* Application::s_Instance = nullptr;//类静态成员s_Instance 初始为null

    //在sandboxapp创建类对象
Application::Application()
{
    HZ_CORE_ASSERT(s_Instance, "Application already exists");
    s_Instance = this;
    //指针=window类型，调用函数的指针
    m_Window =std::unique_ptr<Window>(Window::Create());//创建窗口
    m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//调用SetEventCallback就是调用OnEvent事件设置参数

    unsigned int id;//无符号整数变量
    glGenVertexArrays(1, &id);//顶点数组对象
}
Application::~Application()
{
}

void Application::pushlayer(Layer* layer)
{
    m_Layerstack.PushLayer(layer);
    layer->OnAttach();
}

void Application::pushOverlayer(Layer* overlay)
{
    m_Layerstack.PushOverlay(overlay);
    overlay->OnAttach();

}

void Application::OnEvent(Event& e)//检查
{
    EventDispatcher dispatcher(e);//调度员
    dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose方法与模板WindowCloseEvent比较
    //层
    for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
    {
        (*--it)->OnEvent(e);//逐个向前遍历--onevent是layer类型（可悬停查看）
        if (e.Handled)////层被处理过
            break;

    }
}


	void Application::Run()
	{
        while (m_Running)
        {//初始化窗口
            glClearColor(1, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            for (Layer* layer : m_Layerstack)//遍历所有层
                layer->OnUpdate();//调用层实时更新
            //
            m_Window->OnUpdate();//窗口实时更新
        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//窗口关闭
    {
        m_Running = false;//结束运行
        return true;
    }


}