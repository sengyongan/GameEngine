//���ϵͳ
#pragma once
#include"SceneCamera.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/Font.h"
//��Ԫ��
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/quaternion.hpp>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

namespace Hazel {
    struct IDComponent
    {
        UUID ID;//IDΨһ��ʶ��

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };
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
    struct CircleRendererComponent
    {
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent&) = default;
    };
    struct CameraComponent {
        SceneCamera Camera;//Projection��ViewProjection��ͼ���
        bool Primary = true;//��Ҫ��
        bool FixedAspectRatio = false;//�ݺ��(�ǹ̶����Ӷ�������С���ᱻ���ţ�

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
    struct ScriptComponent
    {
        std::string ClassName;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
    };
    // Forward declaration
    class ScriptableEntity;

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

    struct BoxCollider2DComponent//������ײ��
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
    struct CircleCollider2DComponent//Բ����ײ��
    {
        glm::vec2 Offset = { 0.0f, 0.0f };
        float Radius = 0.5f;

        // TODO(Yan): move into physics material in the future maybe
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        // Storage for runtime
        void* RuntimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };

    struct TextComponent//�ı����
    {
        std::string TextString;//�ı�
        Ref<Font> FontAsset = Font::GetDefault();//������
        glm::vec4 Color{ 1.0f };//��ɫ
        float Kerning = 0.0f;//�־�
        float LineSpacing = 0.0f;//�о�
    };
    //����������ͱ���AllComponents
    template<typename... Component>
    struct ComponentGroup
    {
    };

    using AllComponents =
        ComponentGroup<
        TransformComponent, SpriteRendererComponent, CircleRendererComponent, CameraComponent, ScriptComponent,
        NativeScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, TextComponent
        >;
        

}