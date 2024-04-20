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

        virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; };

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
        FramebufferSpecification m_Specification;//Ö¡»º³å
    };
}