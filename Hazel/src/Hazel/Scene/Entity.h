//实体管理API
#pragma once
#include"Scene.h"
#include"entt.hpp"

#include "Hazel/Core/UUID.h"
#include "Components.h"

namespace Hazel
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        ///////////////////////////////////////////
        template <typename T, typename ... Args>//可变参数
        T& AddComponent(Args&& ...args) {//为实体添加组件
            HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component =  m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);//插入类型t的对象（t为struct component）并将可变参数传给结构体
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)//添加或替换组件
        {
            T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
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
        ////////////////////////
        operator bool() const { return m_EntityHandle != entt::null; }//重写条件判断，如果实体不为空，条件为真
        operator entt::entity() const { return m_EntityHandle; }//因为Entity只是抽象类，entt::entity m_EntityHandle才是实体
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }
        ///Get////////////////////
        UUID GetUUID() { return GetComponent<IDComponent>().ID; }//获取当前实体id

        const std::string& GetName() { return GetComponent<TagComponent>().Tag; }//获取名字
        /////////////////////////
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

