#pragma once
//vector<layer>
#include "Hazel/Layer.h"

#include <vector>

namespace Hazel {

    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();//删除所有层

        void PushLayer(Layer* layer);//添加
        void PushOverlay(Layer* overlay);//推式叠加
        void PopLayer(Layer* layer);//弹出
        void PopOverlay(Layer* overlay);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }

    private:
        std::vector<Layer*> m_Layers;//数组结构
        unsigned int  m_LayerInsertIndex = 0;;//总层数
    };

}