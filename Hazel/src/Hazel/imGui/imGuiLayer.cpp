#include "hzpch.h"
#include"imGuiLayer.h"
#include"imgui.h"
#include"Hazel/Platfrom//Opengl/imguiOpenGLRender.h"
#include"Hazel/Application.h"
//temporary
#include<GLFW/glfw3.h>
#include<glad/glad.h>
namespace Hazel{
    imGuiLayer::imGuiLayer() :Layer("ImGuiLayer")//成员初始化列表:将 Layer 的参数设置为 "ImGuiLayer"
    {
    }

   imGuiLayer::~imGuiLayer()
    {
    }

   void imGuiLayer::OnAttach()
   {

       ImGui::CreateContext();//创建上下文
        
       ImGui::StyleColorsDark();//颜色
       ImGuiIO& io = ImGui::GetIO();//结构体
       ImGui_ImplOpenGL3_Init();//输入输出状态和配置信息
       io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;//拥有鼠标游标
       io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
       //???
       //GLFW 库中的键盘按键常量赋值给对应的 ImGuiKey 枚举值
       io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;//示 Tab 键在当前平台上的键码——枚举
       io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
       io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
       io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
       io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
       io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
       io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
       io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
       io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
       io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
       io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
       io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
       io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
       io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
       io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
       io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
       io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
       io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
       io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
       io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
       io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

       ImGui_ImplOpenGL3_Init("#version 410");
       
   }

   void imGuiLayer::OnDetach()
   {
   }

    void imGuiLayer::OnUpdate()
    {

        ImGuiIO& io = ImGui::GetIO();//结构体
        Application& app=Application::Get();//返回Application实例
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());//程序窗口的大小

        float time = (float)glfwGetTime();//获取当前时间
        io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);//时间间隔
        m_Time = time;//下一次

        //渲染
        ImGui_ImplOpenGL3_NewFrame();//初始化
        ImGui::NewFrame();//新的 ImGui 帧


        static bool show = true;
        ImGui::ShowDemoWindow(&show);//生成窗口
        //渲染
        ImGui::Render();//生成绘制指令
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());//指令发送到 GPU




    }

    void imGuiLayer:: OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnMouseButtonPressedEvent));
        dispatcher.Dispatch< MouseButtonReleasedEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnMouseButtonReleasedEvent));
        dispatcher.Dispatch< MouseMovedEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnMouseMovedEvent));
        dispatcher.Dispatch< MouseScrolledEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnMouseScrolledEvent));
        dispatcher.Dispatch< KeyPressedEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnKeyPressedEvent));
        dispatcher.Dispatch< KeyReleasedEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnKeyReleasedEvent));
        dispatcher.Dispatch< KeyTypedEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnKeyTypedEvent));
        dispatcher.Dispatch< WindowResizeEvent>(HZ_BIND_EVENT_FN(imGuiLayer::OnWindowResizeEvent));
        //事件名如果存在，…………

    }

    bool imGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton()] = true;
        return false;
    }

    bool imGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)//松开按键
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton()] = false;

        return false;
    }

    bool imGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(e.GetX(), e.GetY());
        return false;
    }

    bool imGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += e.GetXOffset();
        io.MouseWheel+= e.GetYOffset();

        return false;
    }

    bool imGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[e.GetKeyCode()] = true;
        //其他键
        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
        return false;
    }

    bool imGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[e.GetKeyCode()] = false;
        return false;
    }

    bool imGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        int keycode = e.GetKeyCode();
        if (keycode > 0 && keycode < 0x10000)
            io.AddInputCharacter((unsigned short)keycode);
        return false;
    }


    bool imGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        glViewport(0, 0, e.GetWidth(), e.GetHeight());//glad
        return false;
    }



}

