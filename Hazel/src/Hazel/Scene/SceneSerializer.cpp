#include "hzpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML {

    template<>
    struct convert<glm::vec3>
    {   //vec3
        static Node encode(const glm::vec3& rhs)//编码：添加到node
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)//解码为rhs
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    //vec4
    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}
namespace Hazel {

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)//重载《《，输出vec3
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)//重载《《，输出vec4
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {   //标题
        out << YAML::BeginMap; // Entity(每次BeginMap文本都会缩进
        out << YAML::Key << "Entity" << YAML::Value << "12837192831273"; // TODO: Entity ID goes here
        //////如果有组件序列化
        if (entity.HasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";//TagComponent:
            out << YAML::BeginMap; // TagComponent

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;//Tag: Camera A

            out << YAML::EndMap; // TagComponent
        }
        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

            out << YAML::EndMap; // TransformComponent
        }
        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthopraghicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthopraghicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthopraghicFarClip();
            out << YAML::EndMap; // Camera

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }
        if (entity.HasComponent<SpriteRendererComponent>())
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent

            auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

            out << YAML::EndMap; // SpriteRendererComponent
        }

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";//未命名
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;//BeginSeq新的序列作为值
        m_Scene->m_Registry.each([&](auto entityID)
            {
                Entity entity = { entityID, m_Scene.get() };
                if (!entity)
                    return;

                SerializeEntity(out, entity);
            });
        out << YAML::EndSeq;//结束序列
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        // Not implemented
        HZ_CORE_ASSERT(false);
    }

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        std::ifstream stream(filepath);//输入文件流stream
        std::stringstream strStream;//字符串流strStream对象
        strStream << stream.rdbuf();//将文件流的内容读取到字符串流中，获取文件流的缓冲区指针

        YAML::Node data = YAML::Load(strStream.str());//将字符串流中的内容加载为YAML节点对象data。
        if (!data["Scene"])//中是否存在名为"Scene"的键
            return false;

        std::string sceneName = data["Scene"].as<std::string>();//将值转换为字符串类型，并存储在变量sceneName中
        HZ_CORE_TRACE("Deserializing scene '{0}'", sceneName);

        auto entities = data["Entities"];//值的所有实体
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO标识符
                //
                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();

                HZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
                //
                Entity deserializedEntity = m_Scene->CreateEntity(name);//创建实体
                //
                auto transformComponent = entity["TransformComponent"];//如果能获取到键
                if (transformComponent)
                {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();
                }
                //
                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();

                    auto& cameraProps = cameraComponent["Camera"];
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                    cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                    cc.Camera.SetOrthopraghicSize(cameraProps["OrthographicSize"].as<float>());
                    cc.Camera.SetOrthopraghicNearClip(cameraProps["OrthographicNear"].as<float>());
                    cc.Camera.SetOrthopraghicFarClip(cameraProps["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }
                //
                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if (spriteRendererComponent)
                {
                    auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
                    src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // Not implemented
        HZ_CORE_ASSERT(false);
        return false;
    }

}