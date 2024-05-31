#pragma once
//管理整个场景的 实体/ 组件 的 创建/销毁/更新
#include "entt.hpp"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Core/UUID.h"

class b2World;

namespace Hazel {

    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);//拷贝场景

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        void OnRuntimeStart();//创建销毁2d刚体
        void OnRuntimeStop();
        //physics
        void OnSimulationStart();
        void OnSimulationStop();
        //
        void OnUpdateEditor(Timestep ts,EditorCamera& camera);
        
        void OnUpdateSimulation(Timestep ts, EditorCamera& camera);

        void OnUpdateRuntime(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);

        void DuplicateEntity(Entity entity);//拷贝实体

        Entity GetEntityByUUID(UUID uuid);//通过id获取实体

        Entity GetPrimaryCameraEntity();//找到主摄像机

        template<typename... Components>
        auto GetAllEntitiesWith()//获取所有具有组件的实体
        {
            return m_Registry.view<Components...>();
        }
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);
        //physics
        void OnPhysics2DStart();
        void OnPhysics2DStop();

        void RenderScene(EditorCamera& camera);
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        b2World* m_PhysicsWorld = nullptr;//物理世界

        std::unordered_map<UUID, entt::entity> m_EntityMap;//实体映射

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };

}