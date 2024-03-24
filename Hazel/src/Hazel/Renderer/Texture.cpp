#include"hzpch.h"
#include"Texture.h"
#include"Renderer.h"
#include"Hazel/Platfrom/Opengl/OpenGLTexture.h"

namespace Hazel {
    Ref<Texture2D> Texture2D::Create(const std::string& path)//pathÎªÍ¼ÏñimageÂ·¾¶
    {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture>(path);//shared_ptr = make_shared
        }
        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;

     }

}