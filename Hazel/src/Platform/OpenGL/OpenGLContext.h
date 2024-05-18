#pragma once
//glfw上下文opengl派生类
#include "Hazel/Renderer/GraphicsContext.h"
struct GLFWwindow;//空间外
namespace Hazel {
    class OpenGLContext :public GraphicsContext {

    public:
        OpenGLContext(GLFWwindow* windowHazel);
        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHazel;//当前窗口

    };
}