#include "hzpch.h"
#include"imGuiLayer.h"
#include"imgui.h"
#include"Hazel/Core/Application.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// Temporary
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel {
    imGuiLayer::imGuiLayer() :Layer("ImGuiLayer")//成员初始化列表:将 Layer 的参数设置为 "ImGuiLayer"
    {
    }

    imGuiLayer::~imGuiLayer()
    {
    }

    void imGuiLayer::OnAttach()
    {
        HZ_PROFILE_FUNCTION();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;                     //启用
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking停靠
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
        io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void imGuiLayer::OnDetach()
    {
        HZ_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void imGuiLayer::OnEvent(Event& e)
    {
        if(m_BlockEvents)//???
        {
            ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;//当前触发的事件类型，和用户输入的事件
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureKeyboard;
        }
    }
    void imGuiLayer::Begin() {
        HZ_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void imGuiLayer::End() {
        HZ_PROFILE_FUNCTION();

        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
  

}
