#pragma once
//imGuiLayer层
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
         void OnAttach() override;//附上
         void OnDetach() override;//取下
         void OnEvent(Event& e);

        void Begin();//开始渲染
        void End();

        void BlockEvents(bool block) { m_BlockEvents = block; }//是否被阻止

        void SetDarkThemeColors();

        uint32_t GetActiveWidgetID() const;//获取活动部件 ID
    private:
        bool m_BlockEvents = true;
        float m_Time = 0.0f;

};
}