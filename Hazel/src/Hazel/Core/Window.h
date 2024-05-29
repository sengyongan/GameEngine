#pragma once
//���ڻ��ࣨ�����ࣩ
#include "Hazel/Events/Event.h"
namespace Hazel {
    //���ڳ�ʼ������
	struct WindowProps
	{
		std::string Title;//����
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "senlongan Engine ",
            uint32_t width = 1600, uint32_t height = 900): Title(title), Width(width), Height(height){}
	};

    // ���ڻ���
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;//EventCallbackFn -�� �ص�����
        //���麯��
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
        //�¼��ص�
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        //֡��fps,����Ϊ30fps
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
        //��ȡ���ش���m_Window
		virtual void* GetNativeWindow() const = 0;
        //��̬�����ܴ������ݣ���������
        static Scope<Window> Create(const WindowProps& props = WindowProps());
    };

}