#pragma once
//�������������� ʵ��/ ��� �� ����/����/����
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

        static Ref<Scene> Copy(Ref<Scene> other);//��������

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        void OnRuntimeStart();//��������2d����
        void OnRuntimeStop();
        //physics
        void OnSimulationStart();
        void OnSimulationStop();
        //
        void OnUpdateEditor(Timestep ts,EditorCamera& camera);
        
        void OnUpdateSimulation(Timestep ts, EditorCamera& camera);

        void OnUpdateRuntime(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);

        void DuplicateEntity(Entity entity);//����ʵ��

        Entity GetEntityByUUID(UUID uuid);//ͨ��id��ȡʵ��

        Entity GetPrimaryCameraEntity();//�ҵ��������

        template<typename... Components>
        auto GetAllEntitiesWith()//��ȡ���о��������ʵ��
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

        b2World* m_PhysicsWorld = nullptr;//��������

        std::unordered_map<UUID, entt::entity> m_EntityMap;//ʵ��ӳ��

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };

}