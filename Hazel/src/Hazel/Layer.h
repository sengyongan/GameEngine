#pragma once
#pragma once
//��
#include "Hazel/Events/Event.h"

namespace Hazel {

    class HAZEL_API Layer{
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer() = default;//Ĭ���۹�

        virtual void OnAttach() {}//��ʼ��
        virtual void OnDetach() {}//����
        virtual void OnUpdate() {}//��ʵʱ����
        virtual void OnImGuiRender() {}//��Ⱦ
        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_DebugName; }//�������÷������ͣ�thisΪ���������ĵ����߿���Ϊ����
    protected:
        std::string m_DebugName;
    };

}