#pragma once
#pragma once
#include"Hazel/core.h"
#include"hzpch.h"
//����ģ��
namespace Hazel {

//�¼����ͣ������¼������ڣ�glfw��imgui�������̣����
	enum class EventType
	{
		None = 0,//index=0
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
//�¼����ࣨ���壩��������ö��
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
//��---ʵ�ִ��麯��
//�¼�����
    //GetEventType�����ϲ�GetStaticType
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
//�¼�����
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
//����
	class Event
	{
	public:
		virtual ~Event() = default;//Ĭ�Ϲ���

		bool Handled = false;//���Ƿ񱻴����
		//���麯��
		virtual EventType GetEventType() const = 0;//��ȡ�¼�����//����GetStaticType()
		virtual const char* GetName() const = 0;//��ȡ����
		virtual int GetCategoryFlags() const = 0;//��ȡ�����־
		//�麯����������Ը���
        virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)//�Ƿ��������
		{
			return GetCategoryFlags() & category;//λ�����������&������������true
		}
	protected:
		bool m_Handled = false;
	};
//����
	class EventDispatcher//�¼��ַ��������жϻص�
	{//����ģ�塪���������������bool���ͺ������������к�������
        template<typename T>
        using EventFn = std::function<bool(T&)>;
	public:
		//m_Event��ʼ��ΪEvent����
        EventDispatcher(Event& event)
            : m_Event(event) {}
		

		// F will be deduced by the compiler
		template<typename T>//����ģ��
		bool Dispatch(EventFn<T>  func)//����������������ģ������
		{//Dispatch<MouseButtonPressedEvent>(MouseButtonPressedEvent����m_Event����event���Ƚ�
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));//��ǰbool��������event�������㲢��ֵ��Handled
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
