#pragma once
//��Դ��壨�����ʲ���
#include <filesystem>
#include "Hazel/Renderer/Texture.h"

namespace Hazel {

    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void OnImGuiRender();
    private:
        std::filesystem::path m_CurrentDirectory;//path�࣬��ǰ�����Ŀ¼��

        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
    };

}