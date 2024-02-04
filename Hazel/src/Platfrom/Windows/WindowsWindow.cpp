#include "hzpch.h"
#include "WindowsWindow.h"
#include"Hazel/core.h"
#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Events/KeyEvent.h"
#include"Hazel/Events/MouseEvent.h"


namespace Hazel {
    //bool是否初始化
	static bool s_GLFWInitiallized = false;
    //
    static void GLFWErrorCallback(int error, const char* description)
    {
        HZ_CORE_ERROR("GLFW  Error  ({0}) : {1}", error, description);
    }

    //接受实际数据
    //window中静态函数实现
// 核心调用——类对象——初始函数（接受数据）——传给GLFW的实际数据——创建窗口
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);//返回类对象：包含WindowsWindow所有函数
	}
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
        Shutdown();
	}
    //接受实际数据
    void WindowsWindow::Init(const WindowProps& props)
    {
        //为实际数据赋值=可被更改数据
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        //日志中的宏
        HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
        //判断是否初始化
        if (s_GLFWInitiallized == false)
        {
            int success = glfwInit();//当glfw在启动时
            //如果success没有成功————关闭系统了

            HZ_CORE_ASSERT(success, "Could not initialize GLFW!");//将输出无法初始化 GLFW！
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitiallized = true;

        }
        //变量=实际窗口
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);//当前上下文
        //GLFW中的函数
        glfwSetWindowUserPointer(m_Window, &m_Data);//将用户自定义的数据与窗口关联起来
        SetVSync(true);

        // GLFW callback
        //设置窗口大小回调函数
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int  height)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                WindowResizeEvent  event(width, height);//ApplicationEvent中窗口调整大小事件
                data.Width = width;
                data.Height = height;
                data.EventCallback(event);

            });//【】后是参数和函数体
        //
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                WindowCloseEvent event;
                data.EventCallback(event);

            });
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                switch (action)
                {
                         case GLFW_PRESS://新闻
                        {  
                            KeyPressedEvent event(key, 0);
                            data.EventCallback(event);
                        
                            break;
                        }
                        case GLFW_RELEASE://发布
                        {
                            KeyReleasedEvent event(key);
                            data.EventCallback(event);
                            break;
                        }
                        case GLFW_REPEAT://重复
                        {
                            KeyPressedEvent event(key,1);
                            data.EventCallback(event);
                            break;
                        }
               
                }
            });
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                switch (action)
                {
                        case GLFW_PRESS:
                        {
                            MouseButtonPressedEvent event(button);
                            data.EventCallback(event);
                            break;

                        }
                        case GLFW_RELEASE:
                        {
                            MouseButtonReleasedEvent event(button);
                            data.EventCallback(event);
                            break;

                        }
                }

            });
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                MouseScrolledEvent event((float)xoffset, (float)yoffset);
                data.EventCallback(event);

            });
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                MouseMovedEvent event((float)xpos, (float)ypos);
                data.EventCallback(event);

            });
    }

	void WindowsWindow::Shutdown()
	{

		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()//更新
	{

		glfwPollEvents();
        glfwSwapBuffers(m_Window);//交换缓冲区
	}
    //帧率
	void WindowsWindow::SetVSync(bool enabled)
	{

		if (enabled)
			glfwSwapInterval(1);//垂直同步
		else
			glfwSwapInterval(0);//禁用

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;//返回VSync bool值
	}

}