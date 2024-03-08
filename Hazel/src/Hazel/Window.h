#pragma once
//windows����
#include "Hazel/Events/Event.h"
namespace Hazel {
    //���ڿɱ���������
	struct WindowProps
	{//3������
		std::string Title;//����
		uint32_t Width;
		uint32_t Height;
        //�ն����
        // ���캯������ʼ��3������=�����ĳ�ʼ��
		WindowProps(const std::string& title = "senlonan Hazel Engine ",uint32_t width = 1600,uint32_t height = 900): Title(title), Width(width), Height(height)//��Ա�б�
		{}
	};
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������
// ���ڻ���
	class Window
	{
	public:
        //����=Event������
		using EventCallbackFn = std::function<void(Event&)>;//�����¼���ģ��̳�enent������������Ϊvoid�� ��������
        //δʵ�ֺ���
        //���麯����������Ҳ������
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		// Window attributes
        //�����¼��ص�
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;//����
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
        //�������ڣ���public������Ԫ
		virtual void* GetNativeWindow() const = 0;
        //��̬�����ܴ������ݣ���������
		static Window* Create(const WindowProps& props = WindowProps());
	};

}