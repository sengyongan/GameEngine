#include"hzpch.h"
#include"WindowsInput.h"
#include"Hazel/Application.h"
#include<GLFW/glfw3.h>
namespace Hazel {
    Input* Input::s_Instance = new WindowsInput();
    bool WindowsInput::IsKeyPressedImpl(int keycode)//是否按下
    {
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());//窗口引用--类型转换
        auto state = glfwGetKey(Window, keycode);//return 两个宏之一
        return state == GLFW_PRESS || state == GLFW_REPEAT;//按下/重复--返回flase为释放
    }
    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(Window, button);
        return state == GLFW_PRESS;
    }
    std::pair<float, float> WindowsInput::GetMousePositionImpl()
    {
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);
        return { (float)xpos,(float)ypos };
    }
    float WindowsInput::GetMouseXImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return x;
    }
    float WindowsInput::GetMouseYImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return y;

    }
}