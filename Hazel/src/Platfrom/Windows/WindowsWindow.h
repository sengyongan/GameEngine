#pragma once
#include "Hazel/Window.h"//�̳�
#include <GLFW/glfw3.h>
//windowsʵ��
namespace Hazel {

	class WindowsWindow : public Window
	{
	public:
        //���캯�������ܴ�������
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
        //����--������application
		void OnUpdate() override;
        //���ش�������
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
        inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }//���ô���ʵ������=�¼�
		void SetVSync(bool enabled) override;//������Ϸ֡��
        //override��ʽ��ָʾ�������еĺ����Ǹ��ǻ����е��麯��
		bool IsVSync() const override;
        //��ȡ���ش���
		virtual void* GetNativeWindow() const { return m_Window; }

//��������������������������������������������������������������������������������������������������������������������������������������������������������������������
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();//�ر�
	private:
		GLFWwindow* m_Window;//GLFW��
		//Scope<GraphicsContext> m_Context;
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������
        //����ʵ�����ݻᴫ�ݸ�GLFW���ɿⴴ������
        //WindowData����
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;//�ṹ����
	};

}


