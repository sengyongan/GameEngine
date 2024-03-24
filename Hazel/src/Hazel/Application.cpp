#include"hzpch.h"
#include "Application.h"
#include"Hazel/Input.h"
#include"Window.h"
#include"Hazel/Renderer/Renderer.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
namespace Hazel {
    //Ŀǰ��OnEvent��������Ϊ����placeholdersռλ������ģ�壨δ֪��
    #define BIND_ENENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//��&Application::x�¼�

    Application* Application::s_Instance = nullptr;//�ྲ̬��Աs_Instance ��ʼΪnull
        //��sandboxapp���������
    Application::Application()
    {
        HZ_CORE_ASSERT(s_Instance, "Application already exists");
        s_Instance = this;
        //ָ��=window���ͣ����ú�����ָ��
        m_Window = std::unique_ptr<Window>(Window::Create());//�������ڣ�����Init��
        m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//m_Window-��EventCallback = std::bind(OnEvent, this, std::placeholders::_1)

        Renderer::Init();//��Ⱦ��ʼ�������������

        m_ImGuiLayer = new imGuiLayer;
        pushOverlayer(m_ImGuiLayer);
        //m_Window->SetVSync(false);
        //unsigned int id;//�޷�����������
        //glGenVertexArrays(1, &id);//�����������
    }
    Application::~Application()
    {
    }

    void Application::pushlayer(Layer* layer)
    {
        m_Layerstack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::pushOverlayer(Layer* overlay)
    {
        m_Layerstack.PushOverlay(overlay);
        overlay->OnAttach();

    }
    //
    void Application::OnEvent(Event& e)//�����ǰe�� <WindowCloseEvent> / <WindowResizeEvent>��һ�£���ת��Ϊ�������
    {   //<WindowCloseEvent>Ϊ ��ʾ ģ����ã���ʾ��T = WindowCloseEvent
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose������ģ��WindowCloseEvent�Ƚ�
        dispatcher.Dispatch <WindowResizeEvent>(BIND_ENENT_FN(OnWindowResize));
        //�������в��OnEvent
        for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }


	void Application::Run()
	{
        while (m_Running)
        {               
            float time = (float)glfwGetTime();//��ǰʱ��
            Timestep timestep = time - m_LastFrameTime;//ʱ��
            m_LastFrameTime = time;//����

            //���û�����ش��ڣ�����Ⱦÿһ��
            if (!m_Minimized)
            {
                for (Layer* layer : m_Layerstack)//ͨ��m_Layerstack�������в�
                    layer->OnUpdate(timestep);//���ò�ʵʱ����
            }
            //���ɿ��Դ���imgui����
            m_ImGuiLayer->Begin();
            for (Layer* layer : m_Layerstack)
                layer->OnImGuiRender();//��Ⱦ
            m_ImGuiLayer->End();

            //����ü�鴥���¼�glfwPollEvents�����Բ��ܷ���m_Minimized��
            m_Window->OnUpdate();//����ʵʱ���£��������壩
        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//���ڹر�
    {
        m_Running = false;//��������
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }


}