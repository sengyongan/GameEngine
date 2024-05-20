#include "hzpch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>

namespace Hazel {
    
    extern const std::filesystem::path g_AssetPath  = "assets";//资产目录(文件夹），assets目录

    ContentBrowserPanel::ContentBrowserPanel()
        : m_CurrentDirectory(g_AssetPath )//初始为assets目录
    {
        m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
    }
    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content Browser");

        if (m_CurrentDirectory != std::filesystem::path(g_AssetPath ))//如果当前目录 ！= assets目录
        {
            if (ImGui::Button("<-"))//有返回按钮
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();//当前目录退回到父节点
            }
        }
        //icon
        static float padding = 16.0f;//背景
        static float thumbnailSize = 128.0f;//缩略图尺寸 
        float cellSize = thumbnailSize + padding;//单元格大小

        float panelWidth = ImGui::GetContentRegionAvail().x;//获取内容区域可用性.x
        int columnCount = (int)(panelWidth / cellSize);//列数
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);
        //
        for (auto& directoryEntry  : std::filesystem::directory_iterator(m_CurrentDirectory)) {//创建当前目录的迭代器，遍历每个条目
            
            const auto& path = directoryEntry.path();//获取到条目路径
            auto relativePath = std::filesystem::relative(path, g_AssetPath );//相对于g_AssetPath 路径
            std::string filenameString = relativePath.filename().string();//相对路径转为字符串
            ImGui::PushID(filenameString.c_str());
            //图标（如果是目录，icon变量为目录图片，否则为条目），为Texture2D传入图片path
            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));//背景颜色
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });//GetRendererID获取图片id，创建图片按钮单元格大小
            //拖放   
            if (ImGui::BeginDragDropSource()) {//开始拖放资源
                const wchar_t* itemPath = relativePath.c_str();//wcslen 即空终止宽字符之前的非空宽字符数
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));//设置有效载荷
                ImGui::EndDragDropSource();
            }
            ImGui::PopStyleColor();
            ////是否悬停 / 是否按下左键 
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (directoryEntry .is_directory()) {//如果当前条目是目录（包含条目）
                    m_CurrentDirectory /= path.filename();//operator/=，让当前目录 /= 点击的按钮条目名，下一次循环将进入这个目录，循环它的条目
                }
            }
            ImGui::TextWrapped(filenameString.c_str());

            ImGui::NextColumn();
            ImGui::PopID();
        }
        ImGui::Columns(1);

        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
        ImGui::SliderFloat("Padding", &padding, 0, 32);

        ImGui::End();
    }
}