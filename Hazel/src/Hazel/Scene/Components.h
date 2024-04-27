//组件系统
#pragma once
#include"SceneCamera.h"
#include"ScriptableEntity.h"
#include<glm/glm.hpp>
namespace Hazel {
    struct TagComponent {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string tag)
            :Tag(tag) {}

    };
    struct TransformComponent {//Transform变换组件
        glm::mat4 Transform = glm::mat4(1.0f);//变换成员
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4 Transform)
            :Transform(Transform) {}
        operator const glm::mat4& () const { return Transform; }//将参数转化为glm::mat4 Transform
        operator glm::mat4& () { return Transform; }//只接受非常量
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