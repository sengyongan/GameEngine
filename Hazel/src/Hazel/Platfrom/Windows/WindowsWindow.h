#pragma once
#include "Hazel/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>
//windows����ϵͳ�Ĵ���
namespace Hazel {
	class WindowsWindow : public Window
	{
	public:
        
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
        
		void OnUpdate() override;
        
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
        inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
        
		inline virtual void* GetNativeWindow() const { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);//��ʼ��
		virtual void Shutdown();//�ر�
	private:
		GLFWwindow* m_Window;//ʵ�ʴ���
        //��ǰ������
        GraphicsContext* m_Context;
        //ʵ�ʴ�������
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


