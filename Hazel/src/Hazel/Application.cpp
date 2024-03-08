#include"hzpch.h"
#include "Application.h"
#include"Hazel/Input.h"
#include"Window.h"
#include<glad/glad.h>
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

    unsigned int id;//�޷�����������
    glGenVertexArrays(1, &id);//�����������
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
        {//��ʼ������
            glClearColor(1, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            for (Layer* layer : m_Layerstack)//�������в�
                layer->OnUpdate();//���ò�ʵʱ����
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