#pragma once
//管理项目配置和资源
#include <string>
#include <filesystem>

#include "Hazel/Core/Base.h"

namespace Hazel {
    /*
    Project:
    Name: Sandbox
    StartScene: "Scenes/Physics2D.hazel"
    AssetDirectory: "Assets"
    ScriptModulePath: "Scripts/Binaries/Sandbox.dll"
    */
    struct ProjectConfig//项目配置
    {
        std::string Name = "Untitled";

        std::filesystem::path StartScene;//开始时场景路径

        std::filesystem::path AssetDirectory;//资产(根文件夹）目录路径
        std::filesystem::path ScriptModulePath;//dll模块路径
    };

    class Project
    {
    public:
        static const std::filesystem::path& GetProjectDirectory()//获取项目目录
        {
            HZ_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->m_ProjectDirectory;
        }

        static std::filesystem::path GetAssetDirectory()//获取资产目录
        {
            HZ_CORE_ASSERT(s_ActiveProject);
            return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
        }

        // TODO(Yan): move to asset manager when we have one
        static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)//获取资产文件系统路径
        {
            HZ_CORE_ASSERT(s_ActiveProject);
            return GetAssetDirectory() / path;//"/"是在资产目录添加/，以便获取完整路径
        }

        ProjectConfig& GetConfig() { return m_Config; }

        static Ref<Project> GetActive() { return s_ActiveProject; }

        static Ref<Project> New();
        static Ref<Project> Load(const std::filesystem::path& path);
        static bool SaveActive(const std::filesystem::path& path);
    private:
        ProjectConfig m_Config;
        std::filesystem::path m_ProjectDirectory;//项目路径

        inline static Ref<Project> s_ActiveProject;//Project项目类对象，当前激活的项目,只存在一个实例
    };

}