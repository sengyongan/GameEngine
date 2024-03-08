#pragma once
//���������ʽ
#include "Hazel/Layer.h"

#include <vector>

namespace Hazel {

    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);//���
        void PushOverlay(Layer* overlay);//��ʽ����
        void PopLayer(Layer* layer);//����
        void PopOverlay(Layer* overlay);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }

    private:
        std::vector<Layer*> m_Layers;
        std::vector<Layer*> ::iterator m_layerInsert;;//���ڲ�ĵ��� ��
    };

}