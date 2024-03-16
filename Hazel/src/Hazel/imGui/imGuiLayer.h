#pragma once
//imGuiLayer层
#include "Hazel/Layer.h"
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
         void OnImGuiRender() override;

        void Begin();//开始渲染
        void End();
    private:
        float m_Time = 0.0f;

};
}