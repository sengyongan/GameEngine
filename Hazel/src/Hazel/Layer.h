#pragma once
#pragma once

#include "Hazel/Events/Event.h"

namespace Hazel {

    class   HAZEL_API   Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}//����
        virtual void OnDetach() {}//ȡ��
        virtual void OnUpdate() {}//��ʵʱ����
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}