#pragma once
//�����¼�����
#include"Hazel/Core/core.h"
#include"hzpch.h"
namespace Hazel {
    //�¼����ͣ����ڣ����̣����
	enum class EventType
	{
		None = 0,//index=0
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
    //�¼����ࣨ���壩
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

//�궨���¼�����    //GetEventType�����ϲ�GetStaticType
#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
//�궨���¼�����
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		virtual ~Event() = default;//Ĭ���۹�

		bool Handled = false;//�¼�������
		//���麯��
		virtual EventType GetEventType() const = 0;//��ȡ�¼�����//����GetStaticType()
		virtual const char* GetName() const = 0;//��ȡ�¼�����
		virtual int GetCategoryFlags() const = 0;//��ȡ�¼�����
		
        virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)//�Ƿ��������
		{
			return GetCategoryFlags() & category;//λ�������
		}
	protected:
		bool m_Handled = false;
	};


	class EventDispatcher//�¼��ַ��������bool�����¼����������¼��ַ���bool����
	{   
        //ģ��--��TΪ�κκ�������
        template<typename T>
        using EventFn = std::function<bool(T&)>;//�������--bool���ͺ���

	public:
		//����
        EventDispatcher(Event& event)//event--event�ࣨ���������ࣩ
            : m_Event(event) {}

        //����ģ��
		template<typename T>
		bool Dispatch(EventFn<T> func)//func--bool���ͺ���-----BIND_ENENT_FN(OnWindowClose)
		{//�ж�event��func�Ƿ�ͬ����
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));//�����͵���func������������event
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;//�����event
	};

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }


}
