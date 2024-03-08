#pragma once
//windows窗口
#include "Hazel/Events/Event.h"
namespace Hazel {
    //窗口可被更改数据
	struct WindowProps
	{//3个变量
		std::string Title;//标题
		uint32_t Width;
		uint32_t Height;
        //终端输出
        // 构造函数：初始化3个变量=参数的初始化
		WindowProps(const std::string& title = "senlonan Hazel Engine ",uint32_t width = 1600,uint32_t height = 900): Title(title), Width(width), Height(height)//成员列表
		{}
	};
//——————————————————————————————————————————————————————————————————————————————————
// 窗口基类
	class Window
	{
	public:
        //参数=Event类类型
		using EventCallbackFn = std::function<void(Event&)>;//接受事件类的（继承enent），返回类型为void的 ——函数
        //未实现函数
        //纯虚函数，基类中也不定义
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		// Window attributes
        //设置事件回调
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;//参数
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
        //公开窗口：非public——友元
		virtual void* GetNativeWindow() const = 0;
        //静态：接受窗口数据，创建窗口
		static Window* Create(const WindowProps& props = WindowProps());
	};

}