#include"hzpch.h"
#include "Application.h"
#include"Hazel/Input.h"
#include"Window.h"
#include"Hazel/Renderer/Renderer.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
namespace Hazel {
    //Ŀǰ��OnEvent��������Ϊ����placeholdersռλ������ģ�壨δ֪��
    #define BIND_ENENT_FN(x)  std::bind(&Application::x, this, std::placeholders::_1)//��&Application::x�¼�

    Application* Application::s_Instance = nullptr;//�ྲ̬��Աs_Instance ��ʼΪnull
        //��sandboxapp���������
    Application::Application()
    {
        HZ_CORE_ASSERT(s_Instance, "Application already exists");
        s_Instance = this;
        //ָ��=window���ͣ����ú�����ָ��
        m_Window =std::unique_ptr<Window>(Window::Create());//��������
        m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//����SetEventCallback���ǵ���OnEvent�¼����ò���
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

    void Application::OnEvent(Event& e)//���
    {
        EventDispatcher dispatcher(e);//����Ա
        dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose������ģ��WindowCloseEvent�Ƚ�
        //��
        for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
        {
            (*--it)->OnEvent(e);//�����ǰ����--onevent��layer���ͣ�����ͣ�鿴��
            if (e.Handled)////�㱻�����
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
            //
            for (Layer* layer : m_Layerstack)//�������в�
                layer->OnUpdate(timestep);//���ò�ʵʱ����
            //
            m_ImGuiLayer->Begin();
            for (Layer* layer : m_Layerstack)
                layer->OnImGuiRender();//��Ⱦ
            m_ImGuiLayer->End();
            //
            m_Window->OnUpdate();//����ʵʱ����

        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//���ڹر�
    {
        m_Running = false;//��������
        return true;
    }


}