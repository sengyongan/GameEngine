#include "hzpch.h"
#include"OpenGLTexture.h"
#include"stb_image.h"
#include<glad/glad.h>

namespace Hazel {
    OpenGLTexture::OpenGLTexture(const std::string& path)
        : m_Path(path)
    {   //加载图像并将其存储为OpenGL纹理的过程
        stbi_set_flip_vertically_on_load(1);//翻转为正确图像
        int width, height, channels;//加载图像文件，并将图像数据存储在data指针中，获取图像的宽度、高度和通道数。
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        HZ_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;
        //
        GLenum internalFormat = 0, dataFormat = 0;//Format格式
        if (channels == 4) {//图片有透明通道
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported");
        //OpenGL创建一个新的纹理对象，并为其分配存储空间，纹理对象的标识符存储在m_RendererID
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
        //设置纹理的过滤方式
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //将图像数据上传到OpenGL纹理对象中
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
        //释放了之前加载的图像数据所占用的内存
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