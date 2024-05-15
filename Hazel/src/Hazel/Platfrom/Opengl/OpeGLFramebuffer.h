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
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;//读取像素
        virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

    private:
        uint32_t m_RendererID = 0;//帧缓冲id
        //
        FramebufferSpecification m_Specification;//帧缓冲规范
        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;//帧缓冲器颜色附件规范数组
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;////帧缓冲器深度空的附件规范////默认移动赋值操作符（将FramebufferTextureSpecification中的TextureFormat设置为format.TextureFormat
        
        std::vector<uint32_t> m_ColorAttachments;//颜色附件（int为id值）
        uint32_t m_DepthAttachment = 0;
    };
}