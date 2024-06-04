#include "hzpch.h"
#include"OpenGLTexture2D.h"
#include"stb_image.h"

namespace Hazel 
{
    namespace Utils {

        static GLenum HazelImageFormatToGLDataFormat(ImageFormat format)//图片通道转为gl数据
        {
            switch (format)
            {
            case ImageFormat::RGB8:  return GL_RGB;
            case ImageFormat::RGBA8: return GL_RGBA;
            }

            HZ_CORE_ASSERT(false);
            return 0;
        }

        static GLenum HazelImageFormatToGLInternalFormat(ImageFormat format)//图片通道转为内部格式
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

        //加载图像并将其存储为OpenGL纹理的过程
        stbi_set_flip_vertically_on_load(1);//翻转为正确图像
        int width, height, channels;//加载图像文件，并将图像数据存储在data指针中，获取图像的宽度、高度和通道数。
        stbi_uc* data = nullptr;
        {
            HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);//可以用来读取图像文件并将其解码为像素数据。
        }
        HZ_CORE_ASSERT(data, "Failed to load image!");

        if (data)//数据存在，创建纹理
        {
            m_IsLoaded = true;

            m_Width = width;
            m_Height = height;
            //channels
            GLenum internalFormat = 0, dataFormat = 0;//Format格式
            if (channels == 4) {//图片有透明通道
                internalFormat = GL_RGBA8;//内部格式
                dataFormat = GL_RGBA;//数据格式
            }
            else if (channels == 3) {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
            }
            m_InternalFormat = internalFormat;
            m_DataFormat = dataFormat;
            HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported");
            //OpenGL创建一个新的纹理对象，并为其分配存储空间，纹理对象的标识符存储在m_RendererID
            glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
            glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
            //设置纹理的过滤方式
            glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //将图像数据上传到OpenGL纹理对象中
            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
            //释放了之前加载的图像数据所占用的内存
            stbi_image_free(data);
        }
    }
    OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
        : m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
    {
        HZ_PROFILE_FUNCTION();
        //Format格式
        m_InternalFormat = Utils::HazelImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = Utils::HazelImageFormatToGLDataFormat(m_Specification.Format);

        //OpenGL创建一个新的纹理对象，并为其分配存储空间，纹理对象的标识符存储在m_RendererID
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
        //设置纹理的过滤方式
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
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);//将数据更新到纹理
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        HZ_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);//solt在OpenGLTexture2D重写中初始为0
    }

}