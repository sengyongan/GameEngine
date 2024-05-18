#include"hzpch.h"
#include"Scene.h"
#include"Components.h"
#include"Hazel/Renderer/Renderer2D.h"
#include"Entity.h"
#include<glm/glm.hpp>
namespace Hazel {

    Scene::Scene()
    {
    }
    Scene::~Scene()
    {
    }
    Entity Scene::CreateEntity(const std::string& name)//��Ϊ������һ���������ô���������ʵ�嶼��һ��ע���
    {
        Entity entity = { m_Registry.create(),this };//����ʵ����{ʵ�壬����}��
        entity.AddComponent< TransformComponent>();//Ϊʵ��������
        auto& tag = entity.AddComponent< TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;//��������ĳ�Ա
        return entity;//����ʵ����
    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }
    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);//��cameraTransform������ɫ��
        //����
        auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//���о����������������ʵ�����
        for (auto entity : group) {
            auto [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
        }
        Renderer2D::EndScene();

    }
    void Scene::OnUpdateRuntime(Timestep ts)
    {
        {
            m_Registry.view<NativeScriptComponent>().each
            (
                [=](auto entity, auto& nsc) {//nsc->��дNativeScriptComponent
                    if (!nsc.Instance) //ָ���ʼΪ��ָ��
                    {
                        nsc.Instance = nsc.InstantiateScript();//
                        nsc.Instance->m_Entity = Entity{ entity, this };//����ı�����ʼΪentity��
                        nsc.Instance->OnCreate();//ͳһ�Ľӿڵ���
                    }
                    nsc.Instance->OnUpdate(ts);//ͳһ�Ľӿڵ���
                }
            );
        }

        //�ҵ����������ʵ��
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;

        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();//���������
            for (auto entity : view) {//��ֵ����߱���
                auto [transform, camera] = view.get< TransformComponent, CameraComponent>(entity);//��ʵ���ȡ�����ֵ������

                if (camera.Primary) {//Ψһ�������ʵ��
                    mainCamera = &camera.Camera;
                    cameraTransform = &transform.GetTransform();
                    break;
                }
            }
        }
        //�ҵ����ж����������ʵ��
        if(mainCamera)
        {
            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);//��cameraTransform������ɫ��
            auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//���о����������������ʵ�����
            for (auto entity : group) {
                auto [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
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
    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            const auto& camera = view.get<CameraComponent>(entity);//��ÿһ�������ȡ���
            if (camera.Primary) {//�����������
                return Entity(entity, this);
            }
        }
        return {};
    }

    //OnComponentAdded��������ʱִ�е�
    template<typename T>
    void Scene::OnComponentAdded(Entity enitty, T& component)
    {
        static_assert(false);
    }
    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity enitty, TransformComponent& component) {}
    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity enitty, CameraComponent& component) {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity enitty, SpriteRendererComponent& component) {}
    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity enitty, TagComponent& component) {}
    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity enitty, NativeScriptComponent& component) {}


}