#pragma once
#include"RendererAPI.h"
namespace Hazel {
    class RendererCommand {
    public:
        inline static void SetClearColor(const glm::vec4& color) {//静态--全局生命周期
            s_RendererAPI->SetClearColor(color);
        }
        inline static void Clear() {
            s_RendererAPI->Clear();
        }

        inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
            s_RendererAPI->DrawIndexed(vertexArray);//调用OpenGLRendererAPI中DrawIndexed
        }
    private:
        static RendererAPI* s_RendererAPI;
    };
}