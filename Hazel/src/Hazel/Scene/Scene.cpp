#include"hzpch.h"
#include"Scene.h"

#include"Components.h"
#include "ScriptableEntity.h"
#include"Hazel/Renderer/Renderer2D.h"
#include"Entity.h"
#include "Hazel/Scripting/ScriptEngine.h"
#include "Hazel/Physics/Physics2D.h"

#include<glm/glm.hpp>
//Physics
#include"box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"//�̶��豸
#include "box2d/b2_polygon_shape.h"//�����
#include "box2d/b2_circle_shape.h"

namespace Hazel {

    /////////////////////////////
    Scene::Scene()
    {
    }
    Scene::~Scene()
    {
        delete m_PhysicsWorld;
    }
    //Copy///////////////////////
    template<typename... Component>//�������
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        ([&]()
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
        }(), ...);
    }
    template<typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template<typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)//��������������
    {

        ([&]()
        {
            if (src.HasComponent<Component>())
                dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());//Ϊ��ǰʵ�壬�滻���
        }(), ...);
    }
    template<typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
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
        CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }
    ////Create / Destroy Entity/////////////////////////////////////////
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

        m_EntityMap[uuid] = entity;//�洢����ϣ

        return entity;//����ʵ����

    }
    void Scene::DestroyEntity(Entity entity)
    {
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.destroy(entity);
    }
    ///PhysicsWorld/////////////////////////////////////////////////////
    void Scene::OnRuntimeStart()//�����ڵ����ť����ʱ
    {
        m_IsRunning = true;

        OnPhysics2DStart();

        // Scripting
        {
            ScriptEngine::OnRuntimeStart(this);//�ڽű����洫�룬����������
            // Instantiate all script entitiesʵ�������нű�ʵ��

            auto view = m_Registry.view<ScriptComponent>();//��ȡ��ÿ������c#�ű���ʵ��
            for (auto e : view)
            {
                Entity entity = { e, this };
                ScriptEngine::OnCreateEntity(entity);//����ʵ���OnCreate
            }
        }
    }
    void Scene::OnRuntimeStop()
    {
        m_IsRunning = false;

        OnPhysics2DStop();

        ScriptEngine::OnRuntimeStop();//���ٽű��������ָ��
    }
    void Scene::OnSimulationStart()
    {
        OnPhysics2DStart();
    }
    void Scene::OnSimulationStop()
    {
        OnPhysics2DStop();
    }
    ///OnUpdate////////////////////////////////////////////////////////
    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)//Physics
    {   
        // Render
        RenderScene(camera);
    }
    void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)//Physics
    {
        if (!m_IsPaused || m_StepFrames-- > 0)
        {
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
        }
        // Render
        RenderScene(camera);
    }
    //��������
    void Scene::OnUpdateRuntime(Timestep ts)
    {
        if (!m_IsPaused || m_StepFrames-- > 0)//û����ͣ / ���� �Ϳ��Ը��½ű�������
        {
            // Update scripts
            {
                // C# Entity OnUpdate
                auto view = m_Registry.view<ScriptComponent>();
                for (auto e : view)
                {
                    Entity entity = { e, this };
                    ScriptEngine::OnUpdateEntity(entity, ts);//�ҵ�����c#�ű���ʵ�壬���ø���
                }
                //
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
    ////////////////////////////////////////////////////////////////////////////////////////
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        if (m_ViewportWidth == width && m_ViewportHeight == height)
            return;

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

    void Scene::Step(int frames)
    {
        m_StepFrames = frames;
    }

    Entity  Scene::DuplicateEntity(Entity entity)//����ʵ��
    {
        std::string name = entity.GetName();
        Entity newEntity = CreateEntity(name);

        CopyComponentIfExists(AllComponents{}, newEntity, entity);

        return newEntity;
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
    ////////////////////////////////////////////////////////////////////////////////////////////
    Entity Scene::FindEntityByName(std::string_view name)
    {
        auto view = m_Registry.view<TagComponent>();
        for (auto entity : view)//ѭ��ÿ����tag��ʵ��
        {
            const TagComponent& tc = view.get<TagComponent>(entity);//��ȡ����ǰʵ���TagComponent
            if (tc.Tag == name)//�ҵ���Ҫ��ȡ��name�Ķ�Ӧʵ�壬����ʵ��
                return Entity{ entity, this };
        }
        return {};
    }

    Entity Scene::GetEntityByUUID(UUID uuid)
    {
        // TODO(Yan): Maybe should be assert
        if (m_EntityMap.find(uuid) != m_EntityMap.end())
            return { m_EntityMap.at(uuid), this };

        return {};
    }

    ///Physics////////////////////////////////////////////////////////////////////////////
    void Scene::OnPhysics2DStart()
    {
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });//gravity������������������
        auto view = m_Registry.view<Rigidbody2DComponent>();//���ÿ���������ʵ��
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef;//���ø�������Ϊ�������������
            bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
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
            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

                b2CircleShape circleShape;
                circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
                circleShape.m_radius = transform.Scale.x * cc2d.Radius;

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = cc2d.Density;
                fixtureDef.friction = cc2d.Friction;
                fixtureDef.restitution = cc2d.Restitution;
                fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }

    }

    void Scene::OnPhysics2DStop()
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;

    }

    void Scene::RenderScene(EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);//��cameraTransform������ɫ��
        // Draw sprites   
        {
            auto group = m_Registry.group< TransformComponent>(entt::get< SpriteRendererComponent>);//���о����������������ʵ�����
            for (auto entity : group) {
                auto [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
                //Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f), (int)entity);
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
        // Draw Text 
        Renderer2D::DrawString("SenLongAn", Font::GetDefault(), glm::mat4(1.0f), glm::vec4(1.0f));
        Renderer2D::DrawString(
            R"(
			// MSDF text shader
#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};
struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};
layout (location = 0) out VertexOutput Output;
layout (location = 2) out flat int v_EntityID;
void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
#type fragment
#version 450 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;
struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};
layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat int v_EntityID;
layout (binding = 0) uniform sampler2D u_FontAtlas;
float screenPxRange() {
	const float pxRange = 2.0; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlas, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}
void main()
{
	vec4 texColor = Input.Color * texture(u_FontAtlas, Input.TexCoord);
	vec3 msd = texture(u_FontAtlas, Input.TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	if (opacity == 0.0)
		discard;
	vec4 bgColor = vec4(0.0);
    o_Color = mix(bgColor, Input.Color, opacity);
	if (o_Color.a == 0.0)
		discard;
	
	o_EntityID = v_EntityID;
}
)"
, Font::GetDefault(), glm::mat4(1.0f), glm::vec4(1.0f));
        Renderer2D::EndScene();
    }

    //OnComponentAdded��������ʱִ�е�/////////////////////////////////////////////////////////
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
    void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component) {}
    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity enitty, NativeScriptComponent& component) {}
    template<>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {}
    template<>
    void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {}
    template<>
    void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component) {}


}