#include "hzpch.h"
#include"OpenGLTexture2D.h"
#include"stb_image.h"

namespace Hazel 
{
    namespace Utils {

        static GLenum HazelImageFormatToGLDataFormat(ImageFormat format)//ͼƬͨ��תΪgl����
        {
            switch (format)
            {
            case ImageFormat::RGB8:  return GL_RGB;
            case ImageFormat::RGBA8: return GL_RGBA;
            }

            HZ_CORE_ASSERT(false);
            return 0;
        }

        static GLenum HazelImageFormatToGLInternalFormat(ImageFormat format)//ͼƬͨ��תΪ�ڲ���ʽ
        {
            switch (format)
            {
            case ImageFormat::RGB8:  return GL_RGB8;
            case ImageFormat::RGBA8: return GL_RGBA8;
            }

            HZ_CORE_ASSERT(false);
            return 0;
        }

    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        : m_Path(path)
    {
        HZ_PROFILE_FUNCTION();

        //����ͼ�񲢽���洢ΪOpenGL����Ĺ���
        stbi_set_flip_vertically_on_load(1);//��תΪ��ȷͼ��
        int width, height, channels;//����ͼ���ļ�������ͼ�����ݴ洢��dataָ���У���ȡͼ��Ŀ�ȡ��߶Ⱥ�ͨ������
        stbi_uc* data = nullptr;
        {
            HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);//����������ȡͼ���ļ����������Ϊ�������ݡ�
        }
        HZ_CORE_ASSERT(data, "Failed to load image!");

        if (data)//���ݴ��ڣ���������
        {
            m_IsLoaded = true;

            m_Width = width;
            m_Height = height;
            //channels
            GLenum internalFormat = 0, dataFormat = 0;//Format��ʽ
            if (channels == 4) {//ͼƬ��͸��ͨ��
                internalFormat = GL_RGBA8;//�ڲ���ʽ
                dataFormat = GL_RGBA;//���ݸ�ʽ
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
            glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //��ͼ�������ϴ���OpenGL���������
            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
            //�ͷ���֮ǰ���ص�ͼ��������ռ�õ��ڴ�
            stbi_image_free(data);
        }
    }
    OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
        : m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
    {
        HZ_PROFILE_FUNCTION();
        //Format��ʽ
        m_InternalFormat = Utils::HazelImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = Utils::HazelImageFormatToGLDataFormat(m_Specification.Format);

        //OpenGL����һ���µ�������󣬲�Ϊ�����洢�ռ䣬�������ı�ʶ���洢��m_RendererID
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
        //��������Ĺ��˷�ʽ
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    }
    OpenGLTexture2D::~OpenGLTexture2D()
    {
        HZ_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
        HZ_PROFILE_FUNCTION();

        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);//�����ݸ��µ�����
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        HZ_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);//solt��OpenGLTexture2D��д�г�ʼΪ0
    }

}