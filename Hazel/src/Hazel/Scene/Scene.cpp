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
    Entity Scene::CreateEntity(const std::string& name)//因为都是在一个场景调用创建方法，实体都在一个注册表
    {
        Entity entity = { m_Registry.create(),this };//创建实体类{实体，场景}，
        entity.AddComponent< TransformComponent>();//为实体添加组件
        auto& tag = entity.AddComponent< TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;//传入组件的成员
        return entity;//返回实体类
    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }
    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);//将cameraTransform传入着色器
        //绘制
        auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//所有具有组件（两个）的实体的组
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
                [=](auto entity, auto& nsc) {//nsc->缩写NativeScriptComponent
                    if (!nsc.Instance) //指针初始为空指针
                    {
                        nsc.Instance = nsc.InstantiateScript();//
                        nsc.Instance->m_Entity = Entity{ entity, this };//基类的变量初始为entity类
                        nsc.Instance->OnCreate();//统一的接口调用
                    }
                    nsc.Instance->OnUpdate(ts);//统一的接口调用
                }
            );
        }

        //找到具有组件的实体
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;

        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();//所有摄像机
            for (auto entity : view) {//赋值给左边变量
                auto [transform, camera] = view.get< TransformComponent, CameraComponent>(entity);//从实体获取组件赋值给变量

                if (camera.Primary) {//唯一的主相机实体
                    mainCamera = &camera.Camera;
                    cameraTransform = &transform.GetTransform();
                    break;
                }
            }
        }
        //找到具有多个组件的组的实体
        if(mainCamera)
        {
            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);//将cameraTransform传入着色器
            auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//所有具有组件（两个）的实体的组
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
            if (!cameraComponent.FixedAspectRatio) {//非固定纵横比
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }
    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            const auto& camera = view.get<CameraComponent>(entity);//从每一个相机获取组件
            if (camera.Primary) {//返回主摄像机
                return Entity(entity, this);
            }
        }
        return {};
    }

    //OnComponentAdded在添加组件时执行的
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