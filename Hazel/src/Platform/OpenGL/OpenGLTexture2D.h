#pragma once
//����opengl������
#include"Hazel/Renderer/Texture.h"
#include<glad/glad.h>

namespace Hazel {
    class OpenGLTexture2D : public Texture2D 
    {
    public:
        OpenGLTexture2D(const std::string& path);
        OpenGLTexture2D(uint32_t width, uint32_t height);
        virtual ~OpenGLTexture2D() ;
        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override {return m_RendererID;}

        virtual void SetData(void* data, uint32_t size = 0) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator== (const Texture& other)const override { //==�Ƚ����������m_RendererID
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;//����otherת��ΪOpenGLTexture2D���ͣ�����ȡ��m_RendererID��Ա����
        };
    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;

    };
}