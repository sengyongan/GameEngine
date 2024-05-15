#include"hzpch.h"
#include"OpeGLFramebuffer.h"
#include<glad/glad.h>
namespace Hazel {
    static const uint32_t s_MaxFramebuffer = 8192;
    
    namespace Utils//实用工具
    {   
        //纹理目标
        static GLenum TextureTarget(bool multisampled)
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;//如果是多重采样，就为多重，否则返回普通
        }
        //创建纹理
        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)//outID纹理标识符，传入数组的指针，count传入数组大小
        {   //glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }
        //绑定纹理
        static void BindTexture(bool multisampled, uint32_t id)
        {
            glBindTexture(TextureTarget(multisampled), id);
        }
        //颜色缓冲附件纹理
        static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                /////纹理对象分配空间
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);////将纹理对象附加到帧缓冲对象的 颜色附件点
        }
        //深度缓冲附件纹理
        static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
            }
            else
            {
                glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }
        //是否是深度格式
        static bool IsDepthFormat(FramebufferTextureFormat format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
            }

            return false;
        }
        //将FramebufferTextureFormat中的格式转化为GL_开头的格式
        static GLenum HazelFBTextureFormatToGL(FramebufferTextureFormat format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
            case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
            }

            HZ_CORE_ASSERT(false);
            return 0;
        }
    }
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////Framebuffer////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    OpeGLFramebuffer::OpeGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec)
    {   //分类存储所有帧缓冲纹理附件格式
        //FramebufferTextureSpecification format初始化列表EditorLayer中的每一个
        for (auto spec : m_Specification.Attachments.Attachments) {//FramebufferTextureSpecification.Attachments数组
            if (!Utils::IsDepthFormat(spec.TextureFormat))//不是深度缓冲
                m_ColorAttachmentSpecifications.emplace_back(spec);//添加到颜色缓冲格式数组
            else
                m_DepthAttachmentSpecification = spec;
            //创建纹理附件
            Invalidate();
        }
    }
    OpeGLFramebuffer::~OpeGLFramebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteFramebuffers(1, &m_DepthAttachment);
    }
    void OpeGLFramebuffer::Invalidate()
    {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteFramebuffers(1, &m_DepthAttachment);
            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }
        glCreateFramebuffers(1, &m_RendererID);//创建帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//绑定

        //Attachments附件/////////////////////////////////////////////////////////////////////////////////////////////
        bool multisample = m_Specification.Samples > 1;//多重采样
        //颜色格式存在
        if (m_ColorAttachmentSpecifications.size())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());//调整容器的大小
            Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());//data获取数组指针，创建size个纹理

            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                Utils::BindTexture(multisample, m_ColorAttachments[i]);//绑定纹理
                switch (m_ColorAttachmentSpecifications[i].TextureFormat)//检查每个颜色各式
                {
                    case FramebufferTextureFormat::RGBA8:
                        Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA,
                            m_Specification.Width, m_Specification.Height, i);
                        break;
                    case FramebufferTextureFormat::RED_INTEGER:
                        Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER,
                            m_Specification.Width, m_Specification.Height, i);
                        break;
                }
            }
        }
        //深度格式存在
        if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
        {
            Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
            Utils::BindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentSpecification.TextureFormat)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
                break;
            }
        }
        //
        if (m_ColorAttachments.size() > 1)
        {
            HZ_CORE_ASSERT(m_ColorAttachments.size() <= 4);
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        //断言如果条件不成立打印右侧,检查是否完整
        HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");//检查帧缓冲对象是否完整
         
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//解除对帧缓冲对象的绑定
    }
    void OpeGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//绑定
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);

    }
    void OpeGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//绑定

    }
    void OpeGLFramebuffer::Resize(uint32_t Width, uint32_t Height)
    {
        if (Width < 0 || Height < 0 || Width > s_MaxFramebuffer || Height > s_MaxFramebuffer) {
            HZ_CORE_WARN("Attempted to resize Framebuffer to {0},{1}", Width, Height);
            return;
        }
        m_Specification.Width = Width;
        m_Specification.Height = Height;
        Invalidate();
    }
    int OpeGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;//存储读取到的颜色值的指针
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);//屏幕上的横纵坐标，读取的宽度和高度，只读取红色分量的整数值,存储为整数类型,
        return pixelData;
    }
    void OpeGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
        HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

        auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
        glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
            Utils::HazelFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
    }
}