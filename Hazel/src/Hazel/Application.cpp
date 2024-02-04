#include"hzpch.h"
#include "Application.h"
#include<GLFW/glfw3.h>
#include"Window.h"

namespace Hazel {
    //Ŀǰ��OnEvent��������Ϊ����placeholdersռλ������ģ�壨δ֪��
#define BIND_ENENT_FN(x)  std::bind(&Application::x, this, std::placeholders::_1)//��&Application::x�¼�
    //��sandboxapp���������
Application::Application()
{//ָ��=window���ͣ����ú�����ָ��
    m_Window =std::unique_ptr<Window>(Window::Create());//��������
    m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//����SetEventCallback���ǵ���OnEvent�¼����ò���
}
Application::~Application()
{
}
void Application::OnEvent(Event& e)//���
{
    EventDispatcher dispatcher(e);//����Ա
    dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose������ģ��WindowCloseEvent�Ƚ�
    //��
    for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
    {
        (*--it)->OnEvent(e);//�����ǰ����
        if (e.Handled)
            break;

    }
}

void Application::pushlayer(Layer* layer)
{
    m_Layerstack.PushLayer(layer);
}

void Application::pushOverlayer(Layer* layer)
{
    m_Layerstack.PushOverlay(layer);
}

	void Application::Run()
	{
        while (m_Running)
        {//��ʼ������
            glClearColor(1, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            for (Layer* layer : m_Layerstack)
                layer->OnUpdate();//��ʵʱ����

            m_Window->OnUpdate();//����ʵʱ����
        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//���ڹر�
    {
        m_Running = false;//��������
        return true;
    }


}