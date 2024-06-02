#pragma once
#include"Window.h"
#include"Hazel/Events/Event.h"
#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Core/LayerStack.h"
#include"Hazel/imGui/imGuiLayer.h"
#include"Hazel/Core/Timestep.h"


//��Ҫ��ڵ㡪������������ʼ��
namespace Hazel {
    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](int index) const
        {
            HZ_CORE_ASSERT(index < Count);
            return Args[index];
        }
    };
    struct ApplicationSpecification//Ӧ�ò�淶
    {
        std::string Name = "Hazel Application";
        std::string WorkingDirectory;//����Ŀ¼
        ApplicationCommandLineArgs CommandLineArgs;//Ӧ�ó��������в��� 
    };

	class  Application//Ĭ�Ͽ�
	{
	public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();
        const ApplicationSpecification& GetSpecification() const { return m_Specification; }

        void SubmitToMainThread(const std::function<void()>& function);// �ύ�����߳�
		//
        void Run();//ʵʱ����
        void OnEvent(Event& e);//ѭ�������¼�

        void pushlayer(Layer* layer);//��Ӳ�
        void pushOverlayer(Layer* overlay);
        void Close();

        imGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

        inline static  Application& Get() { return  *s_Instance; }//����Application�����
        inline Window& GetWindow() { return  *m_Window; }//���ش���
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        void ExecuteMainThreadQueue();//ִ�����̶߳���
    private:
        //
        ApplicationSpecification m_Specification;
        //
        std::unique_ptr<Window> m_Window;//Window�����

        imGuiLayer* m_ImGuiLayer;//imgui��ָ��

        bool m_Running = true;//�Ƿ�����
        bool m_Minimized = false;//�����Ƿ���С��

        LayerStack m_Layerstack;//�ܲ������

        float m_LastFrameTime = 0.0f;//��һ֡ʱ��

        std::vector<std::function<void()>> m_MainThreadQueue;//���̶߳��С�void func��
        std::mutex m_MainThreadQueueMutex;//��̬���߳�
    private:
        static Application* s_Instance;//Application�����
	};
	
    Application* CreateApplication(ApplicationCommandLineArgs args);
    //����Ӧ�ó���ʵ��

}