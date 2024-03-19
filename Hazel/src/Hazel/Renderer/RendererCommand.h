#pragma once
#include"RendererAPI.h"
namespace Hazel {
    class RendererCommand {
    public:
        inline static void SetClearColor(const glm::vec4& color) {//��̬--ȫ����������
            s_RendererAPI->SetClearColor(color);
        }
        inline static void Clear() {
            s_RendererAPI->Clear();
        }

        inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
            s_RendererAPI->DrawIndexed(vertexArray);//����OpenGLRendererAPI��DrawIndexed
        }
    private:
        static RendererAPI* s_RendererAPI;
    };
}