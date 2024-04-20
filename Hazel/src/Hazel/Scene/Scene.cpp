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
        struct TransformComponent {//Transform变换组件
            glm::vec4 Transform;//变换成员
            TransformComponent() = default;
            TransformComponent(const TransformComponent&) = default;
            TransformComponent(const glm::vec4 Transform)
                :Transform(Transform){}
            operator const glm::vec4& () const { return Transform; }//将参数转化为glm::vec4 Transform
            operator  glm::vec4& () { return Transform; }//只接受非常量
        };

        entt::entity entity = m_Registry.create();//实体管理创建实体
        m_Registry.emplace< TransformComponent>(entity, glm::vec4(1.0f));//将组件与实体关联

        m_Registry.on_construct< TransformComponent>().connect<&OnTransformConstruct>();

        if (m_Registry.has< TransformComponent>(entity)) {//检查实体是否具有指定类型的组件
            TransformComponent& transform = m_Registry.get< TransformComponent>(entity);
        }

        auto view = m_Registry.view< TransformComponent>();//所有具有指定类型组件的实体
        for (auto entity : view) {
            TransformComponent& transform = view.get< TransformComponent>(entity);
        }

        auto group = m_Registry.group< TransformComponent>(entt::get< MeshComponent>);//所有具有组件（两个）的实体的组
        for (auto entity : group) {
            auto&[Transform, mesh] = group.get< TransformComponent, MeshComponent>(entity);
        }

    }
    Scene::~Scene()
    {
    }
}