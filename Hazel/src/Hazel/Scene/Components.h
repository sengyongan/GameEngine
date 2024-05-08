//组件系统
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
    struct TransformComponent {//Transform变换组件
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
    struct SpriteRendererComponent {//贴图渲染组件
        glm::vec4 Color;//变换成员

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color)
            :Color(color) {}
    };

    struct CameraComponent {
        SceneCamera Camera;//Projection和ViewProjection视图组成
        bool Primary = true;//首要的
        bool FixedAspectRatio = false;//纵横比(非固定，从而场景大小不会被缩放）

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct NativeScriptComponent//本地脚本
    {
        ScriptableEntity* Instance = nullptr;//基类

        ScriptableEntity* (*InstantiateScript)();//函数指针，返回ScriptableEntity
        void (*DestoryScript) (NativeScriptComponent*);//函数指针

        template<typename T>
        void Bind() {
            InstantiateScript = []() {return static_cast<ScriptableEntity*> (new T());};
            DestoryScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr;};
        }
    };
}