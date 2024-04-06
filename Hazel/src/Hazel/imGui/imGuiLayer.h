#pragma once
//imGuiLayer��
#include "Hazel/Core/Layer.h"
#include"Hazel/Events/KeyEvent.h"
#include"Hazel/Events/MouseEvent.h"
#include"Hazel/Events/ApplicationEvent.h"

namespace Hazel {
    class HAZEL_API imGuiLayer : public Layer
    { 
    public:
        imGuiLayer();
        ~imGuiLayer();
         void OnAttach() override;//����
         void OnDetach() override;//ȡ��

        void Begin();//��ʼ��Ⱦ
        void End();
    private:
        float m_Time = 0.0f;

};
}