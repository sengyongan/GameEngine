#include"Hazel/Renderer/Framebuffer.h"

namespace Hazel {
    class OpeGLFramebuffer : public Framebuffer {
    public:
        OpeGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpeGLFramebuffer();
        
        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t Width, uint32_t  Height) override;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { 
            HZ_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index];
        }
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;//��ȡ����
        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

    private:
        uint32_t m_RendererID = 0;//֡����id
        //
        FramebufferSpecification m_Specification;//֡����淶
        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;//֡��������ɫ�����淶����
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;////֡��������ȿյĸ����淶////Ĭ���ƶ���ֵ����������FramebufferTextureSpecification�е�TextureFormat����Ϊformat.TextureFormat
        
        std::vector<uint32_t> m_ColorAttachments;//��ɫ������intΪidֵ��
        uint32_t m_DepthAttachment = 0;
    };
}