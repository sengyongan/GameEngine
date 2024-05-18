#pragma once
#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>
//windows����ϵͳ �Ĵ���window
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
        GraphicsContext* m_Context;//���������������
        //���ڵ�ǰ����
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;//�ص� ����
		};

		WindowData m_Data;//�ṹ����
	};

}


