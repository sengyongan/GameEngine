#pragma once
#pragma once
//��
#include "Hazel/Events/Event.h"
#include "Hazel/Core/Timestep.h"

namespace Hazel {

    class  Layer{
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() {}//��ʼ����Application::pushlayer����
        virtual void OnDetach() {}//����
        virtual void OnUpdate(Timestep ts) {}//ʵʱ����
        virtual void OnImGuiRender() {}//��Ⱦ
        virtual void OnEvent(Event& event) {}//�����¼�

        const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}