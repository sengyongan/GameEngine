
#include "hzpch.h"
#include"imGuiLayer.h"
#include"imgui.h"
#include"Hazel/Application.h"
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
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();//调用创建上下文
        ImGuiIO& io = ImGui::GetIO(); (void)io;                     //启用
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking停靠
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
//————————————————————————————————————————————————————————————————————————————————
        // ImGui 风格
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);//初始化
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void imGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();//关闭
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();//调用销毁上下文
    }
    void imGuiLayer::OnImGuiRender()
    {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }
    void imGuiLayer::Begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();//新缓冲
    }
    void imGuiLayer::End() {//结束渲染
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
