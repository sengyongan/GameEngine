#include"hzpch.h"
#include"Hazel/Core/Input.h"
#include"Hazel/Core/Application.h"
#include<GLFW/glfw3.h>
namespace Hazel {

    //�Ƿ���  keycodeΪ����ֵ
    bool Input::IsKeyPressed(int keycode)
    {   //Applicationʵ��--window�����--m_Window����ʵ��
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(Window, keycode);//glfw�� ���� return ״̬
        return state == GLFW_PRESS || state == GLFW_REPEAT;//����/�ظ�--����һ������������
    }
    bool Input::IsMouseButtonPressed(int button)
    {
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(Window, button);
        return state == GLFW_PRESS;
    }
    std::pair<float, float> Input::GetMousePosition()
    {
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);
        return { (float)xpos,(float)ypos };
    }
    float Input::GetMouseX()
    {
        auto [x, y] = GetMousePosition();
        return x;
    }
    float Input::GetMouseY()
    {
        auto [x, y] = GetMousePosition();
        return y;

    }
}