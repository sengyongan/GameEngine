#include"hzpch.h"
#include"Scene.h"
#include<glm/glm.hpp>
namespace Hazel {
    static void DoMaths(const glm::vec4 Transform) {

    }
    static void OnTransformConstruct(entt::registry& registry, entt::entity entity) {

    }
    Scene::Scene()
    {
        struct MeshComponent {};
        struct TransformComponent {//Transform�任���
            glm::vec4 Transform;//�任��Ա
            TransformComponent() = default;
            TransformComponent(const TransformComponent&) = default;
            TransformComponent(const glm::vec4 Transform)
                :Transform(Transform){}
            operator const glm::vec4& () const { return Transform; }//������ת��Ϊglm::vec4 Transform
            operator  glm::vec4& () { return Transform; }//ֻ���ܷǳ���
        };

        entt::entity entity = m_Registry.create();//ʵ�������ʵ��
        m_Registry.emplace< TransformComponent>(entity, glm::vec4(1.0f));//�������ʵ�����

        m_Registry.on_construct< TransformComponent>().connect<&OnTransformConstruct>();

        if (m_Registry.has< TransformComponent>(entity)) {//���ʵ���Ƿ����ָ�����͵����
            TransformComponent& transform = m_Registry.get< TransformComponent>(entity);
        }

        auto view = m_Registry.view< TransformComponent>();//���о���ָ�����������ʵ��
        for (auto entity : view) {
            TransformComponent& transform = view.get< TransformComponent>(entity);
        }

        auto group = m_Registry.group< TransformComponent>(entt::get< MeshComponent>);//���о����������������ʵ�����
        for (auto entity : group) {
            auto&[Transform, mesh] = group.get< TransformComponent, MeshComponent>(entity);
        }

    }
    Scene::~Scene()
    {
    }
}