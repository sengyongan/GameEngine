//���ϵͳ
#pragma once
#include"SceneCamera.h"
#include"ScriptableEntity.h"
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
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
                * glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
                * glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });
            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };
    struct SpriteRendererComponent {//��ͼ��Ⱦ���
        glm::vec4 Color;//�任��Ա

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
        ScriptableEntity* Instance = nullptr;//����

        ScriptableEntity* (*InstantiateScript)();//����ָ�룬����ScriptableEntity
        void (*DestoryScript) (NativeScriptComponent*);//����ָ��

        template<typename T>
        void Bind() {
            InstantiateScript = []() {return static_cast<ScriptableEntity*> (new T());};
            DestoryScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr;};
        }
    };
}