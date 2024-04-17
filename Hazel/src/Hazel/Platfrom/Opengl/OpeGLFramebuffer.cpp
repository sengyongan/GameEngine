#include"hzpch.h"
#include"OpeGLFramebuffer.h"
#include<glad/glad.h>
namespace Hazel {
    OpeGLFramebuffer::OpeGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec)
    {
        Invalidate();
    }
    OpeGLFramebuffer::~OpeGLFramebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
    }
    void OpeGLFramebuffer::Invalidate()
    {
        glCreateFramebuffers(1, &m_RendererID);//创建帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//绑定

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);//创建纹理
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);//绑定

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width,m_Specification.Height, 0,GL_RGBA, GL_UNSIGNED_BYTE, nullptr);//纹理对象分配空间
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//线性过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);//将纹理对象附加到帧缓冲对象的 颜色附件点

        glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);//创建纹理
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);///纹理对象分配空间
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, m_DepthAttachment, 0);//将纹理对象附加到帧缓冲对象的深度
        //glTexImage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
        // 	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
        
        HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");//检查帧缓冲对象是否完整
         
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//解除对帧缓冲对象的绑定
    }
    void OpeGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//绑定

    }
    void OpeGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//绑定

    }
}