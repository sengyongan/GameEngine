//��������ʼ��������
#pragma once
#include"entt.hpp"
#include"Hazel/Core/Timestep.h"
namespace Hazel {
    class Entity;//����������ʹ��
    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);
        void OnUpdate(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);//�����ӿڴ�С
    private:
        template<typename T>
        void OnComponentAdded(Entity enitty, T& component);
    private:
        entt::registry m_Registry;//ע�������ʵ�������������
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        //��Ԫ
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;//�����볡���㼶
    };

}