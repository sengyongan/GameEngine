#pragma once
//资源面板（管理资产）
#include <filesystem>
#include "Hazel/Renderer/Texture.h"

namespace Hazel {

    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void OnImGuiRender();
    private:
        std::filesystem::path m_CurrentDirectory;//path类，当前进入的目录，

        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
    };

}