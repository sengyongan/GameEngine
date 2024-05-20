#include "hzpch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>

namespace Hazel {
    
    extern const std::filesystem::path g_AssetPath  = "assets";//�ʲ�Ŀ¼(�ļ��У���assetsĿ¼

    ContentBrowserPanel::ContentBrowserPanel()
        : m_CurrentDirectory(g_AssetPath )//��ʼΪassetsĿ¼
    {
        m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
        m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
    }
    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content Browser");

        if (m_CurrentDirectory != std::filesystem::path(g_AssetPath ))//�����ǰĿ¼ ��= assetsĿ¼
        {
            if (ImGui::Button("<-"))//�з��ذ�ť
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();//��ǰĿ¼�˻ص����ڵ�
            }
        }
        //icon
        static float padding = 16.0f;//����
        static float thumbnailSize = 128.0f;//����ͼ�ߴ� 
        float cellSize = thumbnailSize + padding;//��Ԫ���С

        float panelWidth = ImGui::GetContentRegionAvail().x;//��ȡ�������������.x
        int columnCount = (int)(panelWidth / cellSize);//����
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);
        //
        for (auto& directoryEntry  : std::filesystem::directory_iterator(m_CurrentDirectory)) {//������ǰĿ¼�ĵ�����������ÿ����Ŀ
            
            const auto& path = directoryEntry.path();//��ȡ����Ŀ·��
            auto relativePath = std::filesystem::relative(path, g_AssetPath );//�����g_AssetPath ·��
            std::string filenameString = relativePath.filename().string();//���·��תΪ�ַ���
            ImGui::PushID(filenameString.c_str());
            //ͼ�꣨�����Ŀ¼��icon����ΪĿ¼ͼƬ������Ϊ��Ŀ����ΪTexture2D����ͼƬpath
            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));//������ɫ
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });//GetRendererID��ȡͼƬid������ͼƬ��ť��Ԫ���С
            //�Ϸ�   
            if (ImGui::BeginDragDropSource()) {//��ʼ�Ϸ���Դ
                const wchar_t* itemPath = relativePath.c_str();//wcslen ������ֹ���ַ�֮ǰ�ķǿտ��ַ���
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));//������Ч�غ�
                ImGui::EndDragDropSource();
            }
            ImGui::PopStyleColor();
            ////�Ƿ���ͣ / �Ƿ������ 
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (directoryEntry .is_directory()) {//�����ǰ��Ŀ��Ŀ¼��������Ŀ��
                    m_CurrentDirectory /= path.filename();//operator/=���õ�ǰĿ¼ /= ����İ�ť��Ŀ������һ��ѭ�����������Ŀ¼��ѭ��������Ŀ
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