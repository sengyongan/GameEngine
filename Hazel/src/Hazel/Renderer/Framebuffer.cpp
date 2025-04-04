#include "hzpch.h"
#include"Framebuffer.h"
#include"Renderer.h"
#include"Platform/OpenGL/OpeGLFramebuffer.h"

namespace Hazel {
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpeGLFramebuffer>(spec);//参数结构体
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

    }
}

