#pragma once
//RendererAPI中转
//调用s_RendererAPI，渲染器命令
#include"RendererAPI.h"
namespace Hazel {
    class RenderCommand {
    public:
        inline static void Init() {

            s_RendererAPI->Init();
        }
        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
            s_RendererAPI->SetViewport(x, y, width, height);
        }
        inline static void SetClearColor(const glm::vec4& color) {//静态--全局生命周期
            s_RendererAPI->SetClearColor(color);
        }
        inline static void Clear() {
            s_RendererAPI->Clear();
        }

        inline static void DrawIndexed(const Ref<VertexArray>& vertexArray,uint32_t indexCount = 0) {
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);//调用OpenGLRendererAPI中DrawIndexed
        }

        static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
        {
            s_RendererAPI->DrawLines(vertexArray, vertexCount);
        }

        static void SetLineWidth(float width)
        {
            s_RendererAPI->SetLineWidth(width);
        }
    private:
        static RendererAPI* s_RendererAPI;
    };
}