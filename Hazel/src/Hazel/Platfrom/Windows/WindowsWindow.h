#pragma once
#include "Hazel/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>
//windows操作系统的窗口
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
		virtual void Init(const WindowProps& props);//初始化
		virtual void Shutdown();//关闭
	private:
		GLFWwindow* m_Window;//实际窗口
        //当前上下文
        GraphicsContext* m_Context;
        //实际窗口数据
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;//结构对象
	};

}


