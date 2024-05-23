//���ϵͳ
#pragma once
#include"SceneCamera.h"
#include"ScriptableEntity.h"
#include "Hazel/Renderer/Texture.h"
//��Ԫ��
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

namespace Hazel {
    struct TagComponent {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string tag)
            :Tag(tag) {}

    };
    struct TransformComponent {//Transform�任���
        glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3 translation)
            :Translation(translation) {}

        glm::mat4 GetTransform() const {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };
    struct SpriteRendererComponent {//��ͼ��Ⱦ���
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };//�任��Ա
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color)
            :Color(color) {}
    };

    struct CameraComponent {
        SceneCamera Camera;//Projection��ViewProjection��ͼ���
        bool Primary = true;//��Ҫ��
        bool FixedAspectRatio = false;//�ݺ��(�ǹ̶����Ӷ�������С���ᱻ���ţ�

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct NativeScriptComponent//���ؽű�
    {
        ScriptableEntity* Instance = nullptr;//���ָ࣬���ʼΪ��ָ��

        ScriptableEntity* (*InstantiateScript)();//����ָ�룬����ScriptableEntity
        void (*DestoryScript) (NativeScriptComponent*);//����ָ��

        template<typename T>
        void Bind() {
            InstantiateScript = []() {return static_cast<ScriptableEntity*> (new T());};//ת��Ϊ����
            DestoryScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr;};//ɾ������ָ��
        }
    };
    
    //Physics
    struct Rigidbody2DComponent//����2D���
    {
        enum class BodyType { Static = 0, Dynamic, Kinematic };//�������ͣ���̬����̬���˶�ѧ��
        BodyType Type = BodyType::Static;//Ĭ�Ͼ�̬
        bool FixedRotation = false;//�Ƿ�̶���ת

        // Storage for runtime
        void* RuntimeBody = nullptr;//����ʱ�洢

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };

    struct BoxCollider2DComponent//��ײ��
    {
        glm::vec2 Offset = { 0.0f, 0.0f };//ƫ�����ʹ�С
        glm::vec2 Size = { 0.5f, 0.5f };

        // TODO(Yan): move into physics material in the future maybe
        float Density = 1.0f;//�ܶ�
        float Friction = 0.5f;//Ħ����
        float Restitution = 0.0f;//�黹
        float RestitutionThreshold = 0.5f;//�ָ�ԭ״��ֵ

        // Storage for runtime
        void* RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };
}