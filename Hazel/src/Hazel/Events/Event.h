#pragma once
//所有事件基类
#include"Hazel/core.h"
#include"hzpch.h"
namespace Hazel {
    //事件类型：窗口，键盘，鼠标
	enum class EventType
	{
		None = 0,//index=0
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
    //事件分类（大体）
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

//宏定义事件类型    //GetEventType返回上层GetStaticType
#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
//宏定义事件分类
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		virtual ~Event() = default;//默认折构

		bool Handled = false;//层是否被处理过
		//纯虚函数
		virtual EventType GetEventType() const = 0;//获取事件类型//调用GetStaticType()
		virtual const char* GetName() const = 0;//获取事件名字
		virtual int GetCategoryFlags() const = 0;//获取事件分类
		
        virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)//是否在类别中
		{
			return GetCategoryFlags() & category;//位与运算符
		}
	protected:
		bool m_Handled = false;
	};

	class EventDispatcher//事件分发器（如果bool函数事件成立，将事件分发给bool函数
	{   
        //模板--（T为任何函数名）
        template<typename T>
        using EventFn = std::function<bool(T&)>;//定义别名--bool类型函数
	public:
		//构造
        EventDispatcher(Event& event)
            : m_Event(event) {}

        //函数模板
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{//执行将 m_Event 对象转换为类型为T的引用,传递给func函数。从而调用bool函数，执行Event& event（事件）的函数
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;//传入的bool函数
	};

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }


}
