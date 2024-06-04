#pragma once
//������Ŀ���ú���Դ
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
    struct ProjectConfig//��Ŀ����
    {
        std::string Name = "Untitled";

        std::filesystem::path StartScene;//��ʼʱ����·��

        std::filesystem::path AssetDirectory;//�ʲ�(���ļ��У�Ŀ¼·��
        std::filesystem::path ScriptModulePath;//dllģ��·��
    };

    class Project
    {
    public:
        static const std::filesystem::path& GetProjectDirectory()//��ȡ��ĿĿ¼
        {
            HZ_CORE_ASSERT(s_ActiveProject);
            return s_ActiveProject->m_ProjectDirectory;
        }

        static std::filesystem::path GetAssetDirectory()//��ȡ�ʲ�Ŀ¼
        {
            HZ_CORE_ASSERT(s_ActiveProject);
            return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
        }

        // TODO(Yan): move to asset manager when we have one
        static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)//��ȡ�ʲ��ļ�ϵͳ·��
        {
            HZ_CORE_ASSERT(s_ActiveProject);
            return GetAssetDirectory() / path;//"/"�����ʲ�Ŀ¼���/���Ա��ȡ����·��
        }

        ProjectConfig& GetConfig() { return m_Config; }

        static Ref<Project> GetActive() { return s_ActiveProject; }

        static Ref<Project> New();
        static Ref<Project> Load(const std::filesystem::path& path);
        static bool SaveActive(const std::filesystem::path& path);
    private:
        ProjectConfig m_Config;
        std::filesystem::path m_ProjectDirectory;//��Ŀ·��

        inline static Ref<Project> s_ActiveProject;//Project��Ŀ����󣬵�ǰ�������Ŀ,ֻ����һ��ʵ��
    };

}