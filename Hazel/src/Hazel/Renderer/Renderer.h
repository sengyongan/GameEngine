#pragma once
//图形api
namespace Hazel {
    enum class RendererAPI {//枚举类
        None = 0,OpenGL = 1
    };
    class Renderer {
    public:
        inline static RendererAPI GetAPI() { return s_RendererAPI; };
        static RendererAPI s_RendererAPI;//图形api,s前缀为全局static
    };
}