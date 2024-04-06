#include "hzpch.h"
#include"OpenGLTexture.h"
#include"stb_image.h"

namespace Hazel {
    OpenGLTexture::OpenGLTexture(const std::string& path)
        : m_Path(path)
    {
        HZ_PROFILE_FUNCTION();
        
        //����ͼ�񲢽���洢ΪOpenGL����Ĺ���
        stbi_set_flip_vertically_on_load(1);//��תΪ��ȷͼ��
        int width, height, channels;//����ͼ���ļ�������ͼ�����ݴ洢��dataָ���У���ȡͼ��Ŀ�ȡ��߶Ⱥ�ͨ������
        stbi_uc* data = nullptr;
        {
            HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture::OpenGLTexture(const std::string&");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        HZ_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;
        //
        GLenum internalFormat = 0, dataFormat = 0;//Format��ʽ
        if (channels == 4) {//ͼƬ��͸��ͨ��
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;
        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported");
        //OpenGL����һ���µ�������󣬲�Ϊ�����洢�ռ䣬�������ı�ʶ���洢��m_RendererID
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
        //��������Ĺ��˷�ʽ
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //��ͼ�������ϴ���OpenGL���������
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
        //�ͷ���֮ǰ���ص�ͼ��������ռ�õ��ڴ�
        stbi_image_free(data);
    }
    OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height)
        :m_Width(width), m_Height(height)
    {
        HZ_PROFILE_FUNCTION();

        m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;//Format��ʽ

        //OpenGL����һ���µ�������󣬲�Ϊ�����洢�ռ䣬�������ı�ʶ���洢��m_RendererID
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
        //��������Ĺ��˷�ʽ
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    }
    OpenGLTexture::~OpenGLTexture()
    {
        HZ_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture::SetData(void* data, uint32_t size)
    {
        HZ_PROFILE_FUNCTION();

        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        HZ_CORE_ASSERT(size == m_Width * m_Height * bpp,"Data must be entire texture!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat,GL_UNSIGNED_BYTE,data);//�����ݸ��µ�����
    }

    void OpenGLTexture::Bind(uint32_t slot) const
    {
        HZ_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);//solt��OpenGLTexture��д�г�ʼΪ0
    }

}