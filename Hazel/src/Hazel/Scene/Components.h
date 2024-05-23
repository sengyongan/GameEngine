//组件系统
#pragma once
#include"SceneCamera.h"
#include"ScriptableEntity.h"
#include "Hazel/Renderer/Texture.h"
//四元数
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
    struct TransformComponent {//Transform变换组件
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
    struct SpriteRendererComponent {//贴图渲染组件
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };//变换成员
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

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
        ScriptableEntity* Instance = nullptr;//基类，指针初始为空指针

        ScriptableEntity* (*InstantiateScript)();//函数指针，返回ScriptableEntity
        void (*DestoryScript) (NativeScriptComponent*);//函数指针

        template<typename T>
        void Bind() {
            InstantiateScript = []() {return static_cast<ScriptableEntity*> (new T());};//转化为基类
            DestoryScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr;};//删除基类指针
        }
    };
    
    //Physics
    struct Rigidbody2DComponent//刚体2D组件
    {
        enum class BodyType { Static = 0, Dynamic, Kinematic };//刚体类型（静态，动态，运动学）
        BodyType Type = BodyType::Static;//默认静态
        bool FixedRotation = false;//是否固定旋转

        // Storage for runtime
        void* RuntimeBody = nullptr;//运行时存储

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };

    struct BoxCollider2DComponent//碰撞器
    {
        glm::vec2 Offset = { 0.0f, 0.0f };//偏移量和大小
        glm::vec2 Size = { 0.5f, 0.5f };

        // TODO(Yan): move into physics material in the future maybe
        float Density = 1.0f;//密度
        float Friction = 0.5f;//摩擦力
        float Restitution = 0.0f;//归还
        float RestitutionThreshold = 0.5f;//恢复原状阈值

        // Storage for runtime
        void* RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };
}