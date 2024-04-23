//ʵ�����API
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
        //Ϊʵ��������
        template <typename T, typename ... Args>//�ɱ����
        T& AddComponent(Args&& ...args) {
            HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template <typename T>
        T& GetComponent() {//��ȡ���
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template <typename T>
        bool HasComponent(){//�Ƿ������
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
        }
        operator bool() const { return m_EntityHandle != entt::null; }//��д�����жϣ����ʵ�岻Ϊ�գ�����Ϊ��
    private:
        entt::entity m_EntityHandle{ entt::null};
        Scene* m_Scene = nullptr;
    };
}

