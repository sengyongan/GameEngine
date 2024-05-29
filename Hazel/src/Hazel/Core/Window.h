#pragma once
//窗口基类（抽象类）
#include "Hazel/Events/Event.h"
namespace Hazel {
    //窗口初始化数据
	struct WindowProps
	{
		std::string Title;//标题
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "senlongan Engine ",
            uint32_t width = 1600, uint32_t height = 900): Title(title), Width(width), Height(height){}
	};

    // 窗口基类
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;//EventCallbackFn -》 回调函数
        //纯虚函数
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
        //事件回调
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        //帧率fps,限制为30fps
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
        //获取本地窗口m_Window
		virtual void* GetNativeWindow() const = 0;
        //静态：接受窗口数据，创建窗口
        static Scope<Window> Create(const WindowProps& props = WindowProps());
    };

}