#pragma once
#pragma once
#include"Hazel/core.h"
#include"hzpch.h"
//中心模块
namespace Hazel {

//事件类型：所有事件，窗口（glfw和imgui），键盘，鼠标
	enum class EventType
	{
		None = 0,//index=0
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
//事件分类（大体）————枚举
	enum EventCategory
	{//index = 1.2.4.8.16
		None = 0,
		EventCategoryApplication = BIT(0),
		//
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		//
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};
//宏---实现纯虚函数
//事件类型
    //GetEventType返回上层GetStaticType
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
//事件分类
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
//基类
	class Event
	{
	public:
		virtual ~Event() = default;//默认构造

		bool Handled = false;//层是否被处理过
		//纯虚函数
		virtual EventType GetEventType() const = 0;//获取事件类型//调用GetStaticType()
		virtual const char* GetName() const = 0;//获取名字
		virtual int GetCategoryFlags() const = 0;//获取分类标志
		//虚函数派生类可以更改
        virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)//是否在类别中
		{
			return GetCategoryFlags() & category;//位与运算符（与&）都成立返回true
		}
	protected:
		bool m_Handled = false;
	};
//基类
	class EventDispatcher//事件分发器——判断回调
	{//函数模板—————定义别名bool类型函数（可以所有函数名）
        template<typename T>
        using EventFn = std::function<bool(T&)>;
	public:
		//m_Event初始化为Event对象
        EventDispatcher(Event& event)
            : m_Event(event) {}
		

		// F will be deduced by the compiler
		template<typename T>//函数模板
		bool Dispatch(EventFn<T>  func)//参数————函数模板类型
		{//Dispatch<MouseButtonPressedEvent>(MouseButtonPressedEvent）《m_Event》（event）比较
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));//向当前bool函数传入event，或运算并赋值给Handled
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }


}
