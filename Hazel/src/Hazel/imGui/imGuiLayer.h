#pragma once
//imGuiLayer²ã
#include "Hazel/Layer.h"
#include"Hazel/Events/KeyEvent.h"
#include"Hazel/Events/MouseEvent.h"
#include"Hazel/Events/ApplicationEvent.h"

namespace Hazel {
    class HAZEL_API imGuiLayer : public Layer
    {
    private:
        float m_Time = 0.0f;//

    public:
        imGuiLayer();
        ~imGuiLayer();
        virtual void OnAttach();//¸½ÉÏ
        virtual void OnDetach();//È¡ÏÂ

        virtual void OnUpdate() ;
        virtual void OnEvent(Event& event);
    private:
           bool  OnMouseButtonPressedEvent  (MouseButtonPressedEvent& e);
           bool  OnMouseButtonReleasedEvent  (MouseButtonReleasedEvent& e);
           bool  OnMouseMovedEvent  (MouseMovedEvent& e);
           bool  OnMouseScrolledEvent  (MouseScrolledEvent& e);
           bool  OnKeyPressedEvent  (KeyPressedEvent& e);
           bool  OnKeyReleasedEvent  (KeyReleasedEvent& e);
           bool  OnKeyTypedEvent  (KeyTypedEvent& e);
           bool  OnWindowResizeEvent  (WindowResizeEvent& e);
};
}