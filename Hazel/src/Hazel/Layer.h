#pragma once
#pragma once

#include "Hazel/Events/Event.h"

namespace Hazel {

    class   HAZEL_API   Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}//附上
        virtual void OnDetach() {}//取下
        virtual void OnUpdate() {}//层实时更新
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}