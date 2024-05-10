#pragma once
#pragma once
//层
#include "Hazel/Events/Event.h"
#include "Hazel/Core/Timestep.h"

namespace Hazel {

    class  Layer{
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}//初始化在Application::pushlayer调用
        virtual void OnDetach() {}//销毁
        virtual void OnUpdate(Timestep ts) {}//实时更新
        virtual void OnImGuiRender() {}//渲染
        virtual void OnEvent(Event& event) {}//启用事件

        const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}