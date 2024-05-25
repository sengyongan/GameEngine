#include"hzpch.h"
#include"Scene.h"
#include"Components.h"
#include "ScriptableEntity.h"
#include"Hazel/Renderer/Renderer2D.h"
#include"Entity.h"
#include<glm/glm.hpp>

#include"box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"//�̶��豸
#include "box2d/b2_polygon_shape.h"//�����

namespace Hazel {

    static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)//����ת��Ϊ��ײ��
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
    template<typename Component>//�������
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto view = src.view<Component>();//�ҵ����У�ģ�������������ʵ��
        for (auto e : view)
        {
            UUID uuid = src.get<IDComponent>(e).ID;//��ȡid
            HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
            entt::entity dstEnttID = enttMap.at(uuid);//�ӹ�ϣ���ȡ�³���ʵ��

            auto& component = src.get<Component>(e);//��ȡ���
            dst.emplace_or_replace<Component>(dstEnttID, component);//���û��滻
        }
    }

    template<typename Component>
    static void CopyComponentIfExists(Entity dst, Entity src)//��������������
    {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//Ϊ��ǰʵ�壬�滻���
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other)//������ԭ������
    {
        Ref<Scene> newScene = CreateRef<Scene>();//�³���

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        auto& srcSceneRegistry = other->m_Registry;//Դ����ע���
        auto& dstSceneRegistry = newScene->m_Registry;//Ŀ�곡����ע���
        std::unordered_map<UUID, entt::entity> enttMap;//ʵ��ӳ��

        // Create entities in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();//��ԭ�����鿴����ʵ��
        for (auto e : idView)
        {   //��ȡԴ������id��name
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);//Ϊ�³�������ʵ�壬��ԭ��id��ͬ
            enttMap[uuid] = (entt::entity)newEntity;//id���³�����ʵ��
        }

        // Copy components (except IDComponent and TagComponent)�������
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
    Entity Scene::CreateEntity(const std::string& name)//��Ϊ������һ���������ô���������ʵ�嶼��һ��ע���
    {
        return CreateEntityWithUUID(UUID(), name);//�������ֵ������
    }
    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {

        Entity entity = { m_Registry.create(),this };//����ʵ����{ʵ�壬����}��
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent< TransformComponent>();//Ϊʵ��������
        auto& tag = entity.AddComponent< TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;//��������ĳ�Ա
        return entity;//����ʵ����

    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }
    ///PhysicsWorld/////////////////////////////////////////////////////
    void Scene::OnRuntimeStart()
    {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });//gravity��������������
        auto view = m_Registry.view<Rigidbody2DComponent>();//���ÿ���������ʵ��
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;//���ø�������Ϊ�������������
            bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;
            //��������
            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);//���Դ��ݸ�����
            body->SetFixedRotation(rb2d.FixedRotation);
            rb2d.RuntimeBody = body;
            //���������ײ�����
            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
                //��ײ����״
                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
                //��ײ��
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
        Renderer2D::BeginScene(camera);//��cameraTransform������ɫ��
        // Draw sprites   
        {
            auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//���о����������������ʵ�����
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
    //��������
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
        // Physics
        {
            const int32_t velocityIterations = 6;//�ٶȵ���
            const int32_t positionIterations = 2;//λ�õ���
            m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);//ģ����������

            // Retrieve transform from Box2D
            auto view = m_Registry.view<Rigidbody2DComponent>();//���ÿ���������ʵ��
            for (auto e : view)
            {
                Entity entity = { e, this };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2Body* body = (b2Body*)rb2d.RuntimeBody;//��ȡ�����ĸ���
                const auto& position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
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
            // Draw sprites
            {
                auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//���о����������������ʵ�����
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
            if (!cameraComponent.FixedAspectRatio) {//�ǹ̶��ݺ��
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