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
    void Scene::OnUpdate(Timestep ts)
    {
        {
            m_Registry.view<NativeScriptComponent>().each
            (
                [=](auto entity, auto& nsc) {//nsc->��дNativeScriptComponent
                    if (!nsc.Instance) 
                    {
                        nsc.Instance = nsc.InstantiateScript();
                        nsc.Instance->m_Entity = Entity{ entity, this };
                        nsc.Instance->OnCreate();
                    }
                    nsc.Instance->OnUpdate(ts);
                }
            );
        }

        //�ҵ����������ʵ��
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;

        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view) {//��ֵ����߱���
                auto [transform, camera] = view.get< TransformComponent, CameraComponent>(entity);

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
                auto [transform, sprite] = group.get< TransformComponent, SpriteRendererComponent>(entity);
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