#include "hzpch.h"
#include "Hazel/LayerStack.h"

namespace Hazel {
    LayerStack::LayerStack()
    {
        m_layerInsert = m_Layers.begin();
    }
    LayerStack::~LayerStack()
    {
        for (Layer* layer : m_Layers)//ɾ�����в�
        {
            delete layer;
        }
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_layerInsert=m_Layers.emplace(m_layerInsert, layer);
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_Layers.emplace_back(overlay);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.end() , layer);
        if (it != m_Layers.end() )
        {
            m_Layers.erase(it);
            m_layerInsert--;
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto it = std::find(m_Layers.begin() , m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            m_Layers.erase(it);
        }
    }

}