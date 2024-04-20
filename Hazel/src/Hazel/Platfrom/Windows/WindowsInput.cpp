#include"hzpch.h"
#include"Hazel/Core/Input.h"
#include"Hazel/Core/Application.h"
#include<GLFW/glfw3.h>
namespace Hazel {

    //是否按下  keycode为整形值
    bool Input::IsKeyPressed(int keycode)
    {   //Application实例--window类对象--m_Window窗口实例
        auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(Window, keycode);//glfw库 —— return 状态
        return state == GLFW_PRESS || state == GLFW_REPEAT;//按下/重复--其中一个成立返回真
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