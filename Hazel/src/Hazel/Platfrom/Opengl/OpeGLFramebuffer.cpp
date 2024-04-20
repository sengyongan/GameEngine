#include"hzpch.h"
#include"OpeGLFramebuffer.h"
#include<glad/glad.h>
namespace Hazel {
    static const uint32_t s_MaxFramebuffer = 8192;
    OpeGLFramebuffer::OpeGLFramebuffer(const FramebufferSpecification& spec)
        : m_Specification(spec)
    {
        Invalidate();
    }
    OpeGLFramebuffer::~OpeGLFramebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteFramebuffers(1, &m_ColorAttachment);
        glDeleteFramebuffers(1, &m_DepthAttachment);
    }
    void OpeGLFramebuffer::Invalidate()
    {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteFramebuffers(1, &m_ColorAttachment);
            glDeleteFramebuffers(1, &m_DepthAttachment);
        }
        glCreateFramebuffers(1, &m_RendererID);//����֡����
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//��

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);//��������
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);//��

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width,m_Specification.Height, 0,GL_RGBA, GL_UNSIGNED_BYTE, nullptr);//����������ռ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//���Թ���
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);//��������󸽼ӵ�֡�������� ��ɫ������

        glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);//��������
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);///����������ռ�
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, m_DepthAttachment, 0);//��������󸽼ӵ�֡�����������
        //glTexImage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
        // 	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
        
        HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");//���֡��������Ƿ�����
         
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//�����֡�������İ�
    }
    void OpeGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//��
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);

    }
    void OpeGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);//��

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
}