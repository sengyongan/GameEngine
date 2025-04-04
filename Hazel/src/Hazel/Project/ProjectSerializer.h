#pragma once
//项目管理的序列化
#include "Project.h"

namespace Hazel {

    class ProjectSerializer
    {
    public:
        ProjectSerializer(Ref<Project> project);

        bool Serialize(const std::filesystem::path& filepath);
        bool Deserialize(const std::filesystem::path& filepath);
    private:
        Ref<Project> m_Project;
    };

}