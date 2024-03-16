#pragma once
#pragma once
//层
#include "Hazel/Events/Event.h"

namespace Hazel {

    class HAZEL_API Layer{
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;//默认折构

        virtual void OnAttach() {}//初始化
        virtual void OnDetach() {}//销毁
        virtual void OnUpdate() {}//层实时更新
        virtual void OnImGuiRender() {}//渲染
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; }//常量引用返回类型，this为常量，它的调用者可以为常量
    protected:
        std::string m_DebugName;
    };

}