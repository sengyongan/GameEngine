#pragma once
//纹理中采样精灵
#include"Texture.h"
#include<glm/glm.hpp>

namespace Hazel {
    class SubTexture2D {
    public:
        SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);//左下xy，右上xy坐标，

        const Ref<Texture2D> GetTexture() const { return m_Texture; }
        const glm::vec2* GetTexCoords() const { return m_TexCoords; }
        //纹理，采样的左下坐标，精灵块采样大小，精灵块宽高
        static Ref<SubTexture2D> CreatFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellsize,const glm::vec2& spriteSize = { 1,1 });
    private:
        Ref<Texture2D> m_Texture;
        glm::vec2 m_TexCoords[4];//纹理坐标
    };
}