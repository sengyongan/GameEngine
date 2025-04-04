#pragma once
//纹理opengl派生类
#include"Hazel/Renderer/Texture.h"
#include<glad/glad.h>

namespace Hazel {
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(const TextureSpecification& specification);
        virtual ~OpenGLTexture2D() ;

        virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override {return m_RendererID;}
        
        virtual const std::string& GetPath() const override { return m_Path; }
        
        virtual void SetData(void* data, uint32_t size = 0) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool IsLoaded() const override { return m_IsLoaded; }

        virtual bool operator== (const Texture& other)const override { //==比较两个对象的m_RendererID
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;//参数other转换为OpenGLTexture2D类型，并获取其m_RendererID成员变量
        };
    private:
        TextureSpecification m_Specification;

        std::string m_Path;
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;

    };
}