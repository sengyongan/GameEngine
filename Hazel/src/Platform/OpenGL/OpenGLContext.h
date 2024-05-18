#pragma once
//glfw������opengl������
#include "Hazel/Renderer/GraphicsContext.h"
struct GLFWwindow;//�ռ���
namespace Hazel {
    class OpenGLContext :public GraphicsContext {

    public:
        OpenGLContext(GLFWwindow* windowHazel);
        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHazel;//��ǰ����

    };
}