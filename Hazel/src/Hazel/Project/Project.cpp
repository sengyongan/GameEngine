#include "hzpch.h"
#include "Project.h"

#include "ProjectSerializer.h"

namespace Hazel {

    Ref<Project> Project::New()//创建全局变量-》类对象
    {
        s_ActiveProject = CreateRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        Ref<Project> project = CreateRef<Project>();//新类指针

        ProjectSerializer serializer(project);
        if (serializer.Deserialize(path))//可以反序列化
        {
            project->m_ProjectDirectory = path.parent_path();
            s_ActiveProject = project;
            return s_ActiveProject;
        }

        return nullptr;
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        ProjectSerializer serializer(s_ActiveProject);
        if (serializer.Serialize(path))
        {
            s_ActiveProject->m_ProjectDirectory = path.parent_path();//设置项目目录
            return true;
        }

        return false;
    }

}