//实体管理API
#pragma once
#include"Scene.h"
#include"entt.hpp"

namespace Hazel
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        //为实体添加组件
        template <typename T, typename ... Args>//可变参数
        T& AddComponent(Args&& ...args) {
            HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template <typename T>
        T& GetComponent() {//获取组件
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent(){//是否有组件
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
        }

        template<typename T>
        void RemoveComponent() {
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        operator bool() const { return m_EntityHandle != entt::null; }//重写条件判断，如果实体不为空，条件为真
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        bool operator == (const Entity& other) const {//调用时比较==左右
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;//当前实体==这次被选中的实体
        }
        bool operator != (const Entity& other) const {
            return !(*this == other);
        }

    private:
        entt::entity m_EntityHandle{ entt::null};
        Scene* m_Scene = nullptr;
    };
}

