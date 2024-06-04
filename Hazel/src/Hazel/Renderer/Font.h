#pragma once

#include <filesystem>

#include "Hazel/Core/Base.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel {

    struct MSDFData;

    class Font
    {
    public:
        Font(const std::filesystem::path& font);//构造函数：加载字体文件并生成一个字符的轮廓图像
        ~Font();

        const MSDFData* GetMSDFData() const { return m_Data; }//获取字体渲染的数据
        Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }//获取图集纹理

        static Ref<Font> GetDefault();//获取默认字体texture
    private:
        MSDFData* m_Data;//MSDFData结构体，font的数据
        Ref<Texture2D> m_AtlasTexture;//图集的纹理图像
    };

}