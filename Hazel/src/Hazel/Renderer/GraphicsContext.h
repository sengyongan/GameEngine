#pragma once
//上下文基类
namespace Hazel {
    class GraphicsContext {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;//交换前后缓冲区
    };
}