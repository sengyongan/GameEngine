#pragma once
//֡����--ͼƬ
#include "Hazel/Core/Base.h"
namespace Hazel {
    enum class FramebufferTextureFormat//֡���������ʽ
    {
        None = 0,

        // Color//��ɫ
        RGBA8,
        RED_INTEGER,//��ɫ����

        // Depth/stencil//���/ģ��
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8//depth��Ĭ��ֵ
    };

    struct FramebufferTextureSpecification//֡��������淶
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)//��ʽ
            : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;//�����ʽ����
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification//֡�����������淶
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)//����淶(�����б�
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;//��������
    };

    struct FramebufferSpecification {//֡�������淶
        uint32_t Width, Height;
        uint32_t Samples = 1;//���ز���
        FramebufferAttachmentSpecification Attachments;//�����淶-
        bool SwapChainTarget = false;//������Ŀ��
    };

    ///Framebuffer///////////////////////////////////////////////////////////////////////////////////////////////////////
    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;
        virtual void Bind() = 0 ;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t Width,uint32_t  Height) = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;//��ȡһ�����ص���ɫֵ����ȡxy���λ�õĸ�����ͼ����������

        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;//�������ͼ�����ݣ��������������Ϊָ��ֵ��

        virtual uint32_t  GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;
        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

    };
}