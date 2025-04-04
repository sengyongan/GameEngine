#pragma once
//RendererAPI的opengl派生类
#include"Hazel/Renderer/RendererAPI.h"
namespace Hazel {
    class OpenGLRendererAPI :public RendererAPI {
        virtual void Init() override;

        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void Clear() override;
        //
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)override;//绘制三角形
        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
        //
        virtual void SetLineWidth(float width) override;

    };
}