#include"hzpch.h"
#include "Application.h"
#include"Hazel/Core/Input.h"
#include"Window.h"
#include"Hazel/Renderer/Renderer.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
namespace Hazel {
    //目前绑定OnEvent函数参数为——placeholders占位符的类模板（未知）
    #define BIND_ENENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//绑定&Application::x事件

    Application* Application::s_Instance = nullptr;//类静态成员s_Instance 初始为null
        //在sandboxapp创建类对象
    Application::Application(const std::string& name)
    {
        HZ_PROFILE_FUNCTION();
        //HZ_CORE_ASSERT(s_Instance, "Application already exists");//TODO:???
        s_Instance = this;
        //指针=window类型，调用函数的指针
        m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));//创建窗口（调用Init）
        m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//m_Window-》EventCallback = std::bind(OnEvent, this, std::placeholders::_1)

        Renderer::Init();//渲染初始化————混合

        m_ImGuiLayer = new imGuiLayer;
        pushOverlayer(m_ImGuiLayer);
        //m_Window->SetVSync(false);
        //unsigned int id;//无符号整数变量
        //glGenVertexArrays(1, &id);//顶点数组对象
    }
    Application::~Application()
    {
        HZ_PROFILE_FUNCTION();

    }

    void Application::pushlayer(Layer* layer)
    {
        HZ_PROFILE_FUNCTION();

        m_Layerstack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::pushOverlayer(Layer* overlay)
    {
        HZ_PROFILE_FUNCTION();

        m_Layerstack.PushOverlay(overlay);
        overlay->OnAttach();

    }
    void Application::Close()
    {
        m_Running = false;
    }
    //
    void Application::OnEvent(Event& e)//如果当前e与 <WindowCloseEvent> / <WindowResizeEvent>等一致，就转化为这个类型
    {
        HZ_PROFILE_FUNCTION();

        //<WindowCloseEvent>为 显示 模板调用，表示将T = WindowCloseEvent
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose方法与模板WindowCloseEvent比较
        dispatcher.Dispatch <WindowResizeEvent>(BIND_ENENT_FN(OnWindowResize));
        //遍历所有层的OnEvent
        for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }


	void Application::Run()
	{
        HZ_PROFILE_FUNCTION();
    
        while (m_Running)
        {               
            HZ_PROFILE_SCOPE("RunLoop");

            float time = (float)glfwGetTime();//当前时间
            Timestep timestep = time - m_LastFrameTime;//时差
            m_LastFrameTime = time;//滚动

            //如果没有隐藏窗口，就渲染每一层
            if (!m_Minimized)
            {
                {
                    HZ_PROFILE_SCOPE("layerStack OnUpdate");

                    for (Layer* layer : m_Layerstack)//通过m_Layerstack遍历所有层
                        layer->OnUpdate(timestep);//调用层实时更新
                }
                m_ImGuiLayer->Begin();
                {
                    HZ_PROFILE_SCOPE("layerStack OnImGuiRender");

                    for (Layer* layer : m_Layerstack)
                        layer->OnImGuiRender();//渲染
                }
                m_ImGuiLayer->End();
            }
            //如果放在外面，依旧可以处理imgui界面
            //m_ImGuiLayer->Begin();    //for (Layer* layer : m_Layerstack)//    layer->OnImGuiRender();//渲染    //m_ImGuiLayer->End();

            //会调用检查触发事件glfwPollEvents，所以不能放在m_Minimized里
            m_Window->OnUpdate();//窗口实时更新（交换缓冲）
        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//窗口关闭
    {
        m_Running = false;//结束运行
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        HZ_PROFILE_FUNCTION();

        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }


}