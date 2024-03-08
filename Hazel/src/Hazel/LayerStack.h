#pragma once
//层的数组形式
#include "Hazel/Layer.h"

#include <vector>

namespace Hazel {

    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);//添加
        void PushOverlay(Layer* overlay);//推式叠加
        void PopLayer(Layer* layer);//弹出
        void PopOverlay(Layer* overlay);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }

    private:
        std::vector<Layer*> m_Layers;
        std::vector<Layer*> ::iterator m_layerInsert;;//对于层的迭代 器
    };

}