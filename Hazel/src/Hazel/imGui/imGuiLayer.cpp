#include "hzpch.h"
#include"imGuiLayer.h"
#include"imgui.h"
#include"Hazel/Platfrom//Opengl/imguiOpenGLRender.h"
#include"Hazel/Application.h"
//temporary
#include<GLFW/glfw3.h>
#include<glad/glad.h>
namespace Hazel{
    imGuiLayer::imGuiLayer() :Layer("ImGuiLayer")//��Ա��ʼ���б�:�� Layer �Ĳ�������Ϊ "ImGuiLayer"
    {
    }

   imGuiLayer::~imGuiLayer()
    {
    }

   void imGuiLayer::OnAttach()
   {

       ImGui::CreateContext();//����������
        
       ImGui::StyleColorsDark();//��ɫ
       ImGuiIO& io = ImGui::GetIO();//�ṹ��
       ImGui_ImplOpenGL3_Init();//�������״̬��������Ϣ
       io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;//ӵ������α�
       io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
       //???
       //GLFW ���еļ��̰���������ֵ����Ӧ�� ImGuiKey ö��ֵ
       io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;//ʾ Tab ���ڵ�ǰƽ̨�ϵļ��롪��ö��
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

        ImGuiIO& io = ImGui::GetIO();//�ṹ��
        Application& app=Application::Get();//����Applicationʵ��
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());//���򴰿ڵĴ�С

        float time = (float)glfwGetTime();//��ȡ��ǰʱ��
        io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);//ʱ����
        m_Time = time;//��һ��

        //��Ⱦ
        ImGui_ImplOpenGL3_NewFrame();//��ʼ��
        ImGui::NewFrame();//�µ� ImGui ֡


        static bool show = true;
        ImGui::ShowDemoWindow(&show);//���ɴ���
        //��Ⱦ
        ImGui::Render();//���ɻ���ָ��
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());//ָ��͵� GPU




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
        //�¼���������ڣ���������

    }

    bool imGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton()] = true;
        return false;
    }

    bool imGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)//�ɿ�����
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
        //������
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

