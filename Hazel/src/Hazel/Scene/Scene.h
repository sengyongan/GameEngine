//管理场景初始化，更新
#pragma once
#include"entt.hpp"
#include"Hazel/Core/Timestep.h"
namespace Hazel {
    class Entity;//声明，可以使用
    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);
        void OnUpdate(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);//场景视口大小
    private:
        template<typename T>
        void OnComponentAdded(Entity enitty, T& component);
    private:
        entt::registry m_Registry;//注册表：用于实体管理和组件管理
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        //友元
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;//场景与场景层级
    };

}