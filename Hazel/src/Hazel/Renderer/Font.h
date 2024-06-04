#pragma once

#include <filesystem>

#include "Hazel/Core/Base.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel {

    struct MSDFData;

    class Font
    {
    public:
        Font(const std::filesystem::path& font);//���캯�������������ļ�������һ���ַ�������ͼ��
        ~Font();

        Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }//��ȡͼ������
    private:
        MSDFData* m_Data;//MSDFData�ṹ�壬font������
        Ref<Texture2D> m_AtlasTexture;//ͼ��������ͼ��
    };

}