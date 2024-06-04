#pragma once
//imGuiLayer��
#include "Hazel/Core/Layer.h"
#include"Hazel/Events/KeyEvent.h"
#include"Hazel/Events/MouseEvent.h"
#include"Hazel/Events/ApplicationEvent.h"

namespace Hazel {
    class  imGuiLayer : public Layer
    { 
    public:
        imGuiLayer();
        ~imGuiLayer();
         void OnAttach() override;//����
         void OnDetach() override;//ȡ��
         void OnEvent(Event& e);

        void Begin();//��ʼ��Ⱦ
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }//�Ƿ���ֹ

        void SetDarkThemeColors();

        uint32_t GetActiveWidgetID() const;//��ȡ����� ID
    private:
        bool m_BlockEvents = true;
        float m_Time = 0.0f;

};
}