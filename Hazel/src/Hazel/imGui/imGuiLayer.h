#pragma once
//imGuiLayer层
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
         void OnAttach() override;//附上
         void OnDetach() override;//取下
         void OnEvent(Event& e);

        void Begin();//开始渲染
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }//是否被阻止
    private:
        bool m_BlockEvents = true;
        float m_Time = 0.0f;

};
}