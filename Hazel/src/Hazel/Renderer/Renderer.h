#pragma once
//ͼ��api
namespace Hazel {
    enum class RendererAPI {//ö����
        None = 0,OpenGL = 1
    };
    class Renderer {
    public:
        inline static RendererAPI GetAPI() { return s_RendererAPI; };
        static RendererAPI s_RendererAPI;//ͼ��api,sǰ׺Ϊȫ��static
    };
}