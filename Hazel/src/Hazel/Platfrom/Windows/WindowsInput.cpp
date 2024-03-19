#include"hzpch.h"
#include"WindowsInput.h"
#include"Hazel/Application.h"
#include<GLFW/glfw3.h>
namespace Hazel {

    Input* Input::s_Instance = new WindowsInput();
    //是否按下  keycode为整形值
    bool WindowsInput::IsKeyPressedImpl(int keycode)
    {   //Application实例--window类对象--m_Window窗口实例
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(Window, keycode);//glfw库 —— return 状态
        return state == GLFW_PRESS || state == GLFW_REPEAT;//按下/重复--其中一个成立返回真
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