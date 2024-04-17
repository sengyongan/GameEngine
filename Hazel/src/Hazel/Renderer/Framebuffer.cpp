#include "hzpch.h"
#include"Framebuffer.h"
#include"Renderer.h"
#include"Hazel/Platfrom/Opengl/OpeGLFramebuffer.h"

namespace Hazel {
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpeGLFramebuffer>(spec);//µ˜”√¿‡
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

    }
}

