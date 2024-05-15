#include"hzpch.h"
#include"OpeGLFramebuffer.h"
#include<glad/glad.h>
namespace Hazel {
    static const uint32_t s_MaxFramebuffer = 8192;
    
    namespace Utils//ʵ�ù���
    {   
        //����Ŀ��
        static GLenum TextureTarget(bool multisampled)
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;//����Ƕ��ز�������Ϊ���أ����򷵻���ͨ
        }
        //��������
        static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)//outID�����ʶ�������������ָ�룬count���������С
        {   //glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }
        //������
        static void BindTexture(bool multisampled, uint32_t id)
        {
            glBindTexture(TextureTarget(multisampled), id);
        }
        //��ɫ���帽������
        static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                /////����������ռ�
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
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);////��������󸽼ӵ�֡�������� ��ɫ������
        }
        //��Ȼ��帽������
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
        //�Ƿ�����ȸ�ʽ
        static bool IsDepthFormat(FramebufferTextureFormat format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
            }

            return false;
        }
        //��FramebufferTextureFormat�еĸ�ʽת��ΪGL_��ͷ�ĸ�ʽ
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
    {   //����洢����֡������������ʽ
        //FramebufferTextureSpecification format��ʼ���б�EditorLayer�е�ÿһ��
        for (auto spec : m_Specification.Attachments.Attachments) {//FramebufferTextureSpecification.Attachments����
            if (!Utils::IsDepthFormat(spec.TextureFormat))//������Ȼ���
                m_ColorAttachmentSpecifications.emplace_back(spec);//��ӵ���ɫ�����ʽ����
            else
                m_DepthAttachmentSpecification = spec;
            //����������
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
        glCreateFramebuffers(1, &m_RendererID);//����֡����
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//��

        //Attachments����/////////////////////////////////////////////////////////////////////////////////////////////
        bool multisample = m_Specification.Samples > 1;//���ز���
        //��ɫ��ʽ����
        if (m_ColorAttachmentSpecifications.size())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());//���������Ĵ�С
            Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());//data��ȡ����ָ�룬����size������

            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                Utils::BindTexture(multisample, m_ColorAttachments[i]);//������
                switch (m_ColorAttachmentSpecifications[i].TextureFormat)//���ÿ����ɫ��ʽ
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
        //��ȸ�ʽ����
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
        //�������������������ӡ�Ҳ�,����Ƿ�����
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
    int OpeGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;//�洢��ȡ������ɫֵ��ָ��
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);//��Ļ�ϵĺ������꣬��ȡ�Ŀ�Ⱥ͸߶ȣ�ֻ��ȡ��ɫ����������ֵ,�洢Ϊ��������,
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