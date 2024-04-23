//���ϵͳ
#pragma once
#include"SceneCamera.h"
#include<glm/glm.hpp>
namespace Hazel {
    struct TagComponent {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string tag)
            :Tag(tag) {}

    };
    struct TransformComponent {//Transform�任���
        glm::mat4 Transform = glm::mat4(1.0f);//�任��Ա
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4 Transform)
            :Transform(Transform) {}
        operator const glm::mat4& () const { return Transform; }//������ת��Ϊglm::mat4 Transform
        operator glm::mat4& () { return Transform; }//ֻ���ܷǳ���
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
}