#include "hzpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h> 
#include<glad/glad.h>
#include <gl/GL.h>
namespace Hazel {
    OpenGLContext::OpenGLContext(GLFWwindow* windowHazel):m_WindowHazel(windowHazel)
    {
        HZ_CORE_ASSERT(windowHazel,"Window hadle is null")
    }
    void OpenGLContext::Init()
    {
        HZ_PROFILE_FUNCTION();

        glfwMakeContextCurrent(m_WindowHazel);//当前上下文
        int stauts = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);//获取程序地址
        HZ_CORE_ASSERT(stauts, "Failed to initialize glad");
        HZ_CORE_INFO("OpenGL Info:");
        HZ_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
        HZ_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
        HZ_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

    }
    void OpenGLContext::SwapBuffers()
    {
        HZ_PROFILE_FUNCTION();

        glfwSwapBuffers(m_WindowHazel);//交换缓冲区

    }
}