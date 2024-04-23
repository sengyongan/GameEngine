#include"hzpch.h"
#include"Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include"Entity.h"
#include<glm/glm.hpp>
namespace Hazel {
    static void DoMaths(const glm::mat4 Transform) {

    }
    static void OnTransformConstruct(entt::registry& registry, entt::entity entity) {

    }
    Scene::Scene()
    {
#if 0
        entt::entity entity = m_Registry.create();//ʵ�������ʵ��
        m_Registry.emplace< TransformComponent>(entity, glm::mat4(1.0f));//�������ʵ�����

        //��TransformComponent������ʱ�����Ӻ�����Ϊ�ص�����
        m_Registry.on_construct< TransformComponent>().connect<&OnTransformConstruct>();

        //���ʵ���Ƿ����ָ�����͵����
        if (m_Registry.has< TransformComponent>(entity)) {
            TransformComponent& transform = m_Registry.get< TransformComponent>(entity);
        }
        //���о���ָ�����������ʵ��
        auto view = m_Registry.view< TransformComponent>();
        for (auto entity : view) {
            TransformComponent& transform = view.get< TransformComponent>(entity);
        }
        //���о����������������ʵ�����
        auto group = m_Registry.group< TransformComponent>(entt::get< MeshComponent>);
        for (auto entity : group) {
            auto&[Transform, mesh] = group.get< TransformComponent, MeshComponent>(entity);
            
        }
#endif
    }
    Scene::~Scene()
    {
    }
    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = { m_Registry.create(),this };//����ʵ����
        entity.AddComponent< TransformComponent>();//Ϊʵ��������
        auto& tag = entity.AddComponent< TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;//��������ĳ�Ա
        return entity;//����ʵ����
    }
    void Scene::OnUpdate(Timestep ts)
    {
        //�ҵ����������ʵ��
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;

        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {//��ֵ����߱���
                auto& [transform, camera] = view.get< TransformComponent, CameraComponent>(entity);

                if (camera.Primary) {//Ψһ�������ʵ��
                    mainCamera = &camera.Camera;
                    cameraTransform = &transform.Transform;
                    break;
                }
            }
        }
        //�ҵ����ж����������ʵ��
        if(mainCamera)
        {
            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);
            auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//���о����������������ʵ�����
            for (auto entity : group) {
                auto& [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawQuad(transform, sprite.Color);
            }
            Renderer2D::EndScene();
        }
    }
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio) {//�ǹ̶��ݺ��
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    } 
}