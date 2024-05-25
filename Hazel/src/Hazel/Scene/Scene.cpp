#include"hzpch.h"
#include"Scene.h"
#include"Components.h"
#include "ScriptableEntity.h"
#include"Hazel/Renderer/Renderer2D.h"
#include"Entity.h"
#include<glm/glm.hpp>

#include"box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"//固定设备
#include "box2d/b2_polygon_shape.h"//多边形

namespace Hazel {

    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)//刚体转化为碰撞体
    {
        switch (bodyType)
        {
            case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
            case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
            case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
        }

        HZ_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }
    /////////////////////////////
    Scene::Scene()
    {
    }
    Scene::~Scene()
    {
    }
    ////////////////////////////////
    template<typename Component>//复制组件
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto view = src.view<Component>();//找到具有，模板参数，的所有实体
        for (auto e : view)
        {
            UUID uuid = src.get<IDComponent>(e).ID;//获取id
            HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
            entt::entity dstEnttID = enttMap.at(uuid);//从哈希表获取新场景实体

            auto& component = src.get<Component>(e);//获取组件
            dst.emplace_or_replace<Component>(dstEnttID, component);//放置或替换
        }
    }

    template<typename Component>
    static void CopyComponentIfExists(Entity dst, Entity src)//复制组件如果存在
    {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//为当前实体，替换组件
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other)//拷贝（原场景）
    {
        Ref<Scene> newScene = CreateRef<Scene>();//新场景

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;//源场景注册表
        auto& dstSceneRegistry = newScene->m_Registry;//目标场景的注册表
        std::unordered_map<UUID, entt::entity> enttMap;//实体映射

        // Create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();//在原场景查看所有实体
        for (auto e : idView)
        {   //获取源场景的id和name
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);//为新场景创建实体，和原，id相同
            enttMap[uuid] = (entt::entity)newEntity;//id，新场景的实体
        }

        // Copy components (except IDComponent and TagComponent)拷贝组件
        CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }
    /////////////////////////////////////////////////////////
    Entity Scene::CreateEntity(const std::string& name)//因为都是在一个场景调用创建方法，实体都在一个注册表
    {
        return CreateEntityWithUUID(UUID(), name);//返回随机值，名字
    }
    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {

        Entity entity = { m_Registry.create(),this };//创建实体类{实体，场景}，
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent< TransformComponent>();//为实体添加组件
        auto& tag = entity.AddComponent< TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;//传入组件的成员
        return entity;//返回实体类

    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }
    ///PhysicsWorld/////////////////////////////////////////////////////
    void Scene::OnRuntimeStart()
    {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });//gravity，创建物理世界
        auto view = m_Registry.view<Rigidbody2DComponent>();//检查每个带刚体的实体
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;//设置刚体属性为自身组件的属性
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;
            //创建刚体
            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);//属性传递给刚体
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;
            //如果存在碰撞体组件
            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
                //碰撞体形状
                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
                //碰撞体
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }
    void Scene::OnRuntimeStop()
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }
    ////////////////////////////////////////////////////////////////////
    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);//将cameraTransform传入着色器
        // Draw sprites   
        {
            auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//所有具有组件（两个）的实体的组
            for (auto entity : group) {
                auto [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }
        }
        // Draw circles
        {
            auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view)
            {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }
        }
        Renderer2D::EndScene();

    }
    //场景更新
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
        // Physics
        {
            const int32_t velocityIterations = 6;//速度迭代
            const int32_t positionIterations = 2;//位置迭代
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);//模拟物理世界

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2DComponent>();//检查每个带刚体的实体
            for (auto e : view)
            {
                Entity entity = { e, this };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2Body* body = (b2Body*)rb2d.RuntimeBody;//获取创建的刚体
                const auto& position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
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
            // Draw sprites
            {
                auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//所有具有组件（两个）的实体的组
                for (auto entity : group) {
                    auto [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
                    Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
                }
            }
            // Draw circles
            {
                auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                for (auto entity : view)
                {
                    auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                    Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
                }
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
    void Scene::DuplicateEntity(Entity entity)
    {
        std::string name = entity.GetName();
        Entity newEntity = CreateEntity(name);

        CopyComponentIfExists<TransformComponent>(newEntity, entity);
        CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
        CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
        CopyComponentIfExists<CameraComponent>(newEntity, entity);
        CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
        CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
        CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
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
        //static_assert(false);
    }
    template<>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {}
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
    void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {}
    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity enitty, TagComponent& component) {}
    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity enitty, NativeScriptComponent& component) {}
    template<>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {}
    template<>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {}



}