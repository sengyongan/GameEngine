#pragma once
//所有事件基类
#include"Hazel/Core/core.h"
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

		bool Handled = false;//事件触发器
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
        EventDispatcher(Event& event)//event--event类（可以派生类）
            : m_Event(event) {}

        //函数模板
		template<typename T>
		bool Dispatch(EventFn<T> func)//func--bool类型函数-----BIND_ENENT_FN(OnWindowClose)
		{//判断event和func是否同类型
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));//成立就调用func函数，并传入event
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;//传入的event
	};

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }


}
