#include "hzpch.h"
#include "WindowsWindow.h"
#include"Hazel/Core/Base.h"

#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Events/KeyEvent.h"
#include"Hazel/Events/MouseEvent.h"
#include "Hazel/Platfrom/Opengl/OpenGLContext.h"

#include<glad/glad.h>

namespace Hazel {
    
	static bool s_GLFWInitiallized = false;
    
    static void GLFWErrorCallback(int error, const char* description)
    {
        HZ_CORE_ERROR("GLFW  Error  ({0}) : {1}", error, description);
    }

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);//创建新的窗口，并传入props数据
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
        HZ_PROFILE_FUNCTION();
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
        HZ_PROFILE_FUNCTION();

        Shutdown();
	}
    
    void WindowsWindow::Init(const WindowProps& props)
    {
        HZ_PROFILE_FUNCTION();

        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        //判断是否初始化
        if (s_GLFWInitiallized == false)
        {
            HZ_PROFILE_SCOPE("glfwInit");

            int success = glfwInit();//调用glfwInit
            HZ_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitiallized = true;

        }
        //实际窗口创建，并赋予WindowProps中所有数据
        {
            HZ_PROFILE_SCOPE("glfwCreateWindow");
            m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        }
        m_Context = new OpenGLContext(m_Window);//opengl的上下文
        m_Context->Init();//glfwMakeContextCurrent

        //将用户自定义的数据与窗口关联起来Title，Width,Height
        glfwSetWindowUserPointer(m_Window, &m_Data);

        SetVSync(true);

//回调Callback/////////////////////////////////////////////////////////////////////////////////
        //触发事件回调————注意：（window，[](){}）
        //[]是lambda表达式，表示用户自定义函数
        //调用回调时自动赋值int width, int  height
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int  height)
            {               
                HZ_CORE_WARN("{0},{1}", width, height);
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);//获取当前窗口
                WindowResizeEvent event(width, height);
                data.Width = width;//这里调用EventCallback（WindowResizeEvent()），因为EventCallback =BIND_ENENT_FN(OnEvent)
                data.Height = height;//上面替换：BIND_ENENT_FN(OnEvent)（参数=WindowResizeEvent（））
                data.EventCallback(event);//所以实则调用OnEvent（WindowResizeEvent()）
            });
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                WindowCloseEvent event;
                data.EventCallback(event);//这里调用EventCallback（WindowCloseEvent）

            });
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                switch (action)
                {
                         case GLFW_PRESS://按下
                        {  
                            KeyPressedEvent event(key, 0);
                            data.EventCallback(event);
                        
                            break;
                        }
                        case GLFW_RELEASE:
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
        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)//输入字符串
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                KeyTypedEvent event(keycode);
                data.EventCallback(event);

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
            { std::cout << "Y offset: " << yoffset << std::endl;
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
        HZ_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
        HZ_PROFILE_FUNCTION();

		glfwPollEvents();//检查有没有触发什么事件
        m_Context->SwapBuffers();//交换缓冲，显示到屏幕
	}
    
	void WindowsWindow::SetVSync(bool enabled)
	{
        HZ_PROFILE_FUNCTION();

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