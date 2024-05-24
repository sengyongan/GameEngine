//ʵ�����API
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
        template <typename T, typename ... Args>//�ɱ����
        T& AddComponent(Args&& ...args) {//Ϊʵ��������
            HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component =  m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);//��������t�Ķ���tΪstruct component�������ɱ���������ṹ��
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)//��ӻ��滻���
        {
            T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template <typename T>
        T& GetComponent() {//��ȡ���
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent(){//�Ƿ������
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
        }

        template<typename T>
        void RemoveComponent() {
            HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }
        ////////////////////////
        operator bool() const { return m_EntityHandle != entt::null; }//��д�����жϣ����ʵ�岻Ϊ�գ�����Ϊ��
        operator entt::entity() const { return m_EntityHandle; }//��ΪEntityֻ�ǳ����࣬entt::entity m_EntityHandle����ʵ��
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }
        ///Get////////////////////
        UUID GetUUID() { return GetComponent<IDComponent>().ID; }//��ȡ��ǰʵ��id

        const std::string& GetName() { return GetComponent<TagComponent>().Tag; }//��ȡ����
        /////////////////////////
        bool operator == (const Entity& other) const {//����ʱ�Ƚ�==����
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;//��ǰʵ��==��α�ѡ�е�ʵ��
        }
        bool operator != (const Entity& other) const {
            return !(*this == other);
        }

    private:
        entt::entity m_EntityHandle{ entt::null};
        Scene* m_Scene = nullptr;
    };
}

