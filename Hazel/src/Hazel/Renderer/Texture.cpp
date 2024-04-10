#include"hzpch.h"
#include"Texture.h"
#include"Renderer.h"
#include"Hazel/Platfrom/Opengl/OpenGLTexture2D.h"

namespace Hazel {
    Ref<Texture2D> Texture2D::Create(const std::string& path)//pathÎªÍ¼ÏñimageÂ·¾¶
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);//shared_ptr = make_shared
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

     }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width,height);//shared_ptr = make_shared
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

    }

}