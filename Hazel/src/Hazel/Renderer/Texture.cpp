#include"hzpch.h"
#include"Texture.h"
#include"Renderer.h"
#include"Platform/OpenGL/OpenGLTexture2D.h"

namespace Hazel {
    Ref<Texture2D> Texture2D::Create(const std::string& path)//pathΪͼ��image·��
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);//shared_ptr = make_shared
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

     }

    Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(specification);//shared_ptr = make_shared
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

    }

}