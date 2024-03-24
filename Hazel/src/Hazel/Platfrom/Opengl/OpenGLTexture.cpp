#include "hzpch.h"
#include"OpenGLTexture.h"
#include"stb_image.h"
#include<glad/glad.h>

namespace Hazel {
    OpenGLTexture::OpenGLTexture(const std::string& path)
        : m_Path(path)
    {   //����ͼ�񲢽���洢ΪOpenGL����Ĺ���
        stbi_set_flip_vertically_on_load(1);//��תΪ��ȷͼ��
        int width, height, channels;//����ͼ���ļ�������ͼ�����ݴ洢��dataָ���У���ȡͼ��Ŀ�ȡ��߶Ⱥ�ͨ������
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
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
        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported");
        //OpenGL����һ���µ�������󣬲�Ϊ�����洢�ռ䣬�������ı�ʶ���洢��m_RendererID
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
        //��������Ĺ��˷�ʽ
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //��ͼ�������ϴ���OpenGL���������
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
        //�ͷ���֮ǰ���ص�ͼ��������ռ�õ��ڴ�
        stbi_image_free(data);
    }
    OpenGLTexture::~OpenGLTexture()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }

}