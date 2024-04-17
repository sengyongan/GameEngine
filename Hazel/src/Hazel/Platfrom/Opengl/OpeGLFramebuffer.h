#include"Hazel/Renderer/Framebuffer.h"

namespace Hazel {
    class OpeGLFramebuffer : public Framebuffer {
    public:
        OpeGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpeGLFramebuffer();
        
        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; };

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

    private:
        uint32_t m_RendererID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FramebufferSpecification m_Specification;//Ö¡»º³å
    };
}