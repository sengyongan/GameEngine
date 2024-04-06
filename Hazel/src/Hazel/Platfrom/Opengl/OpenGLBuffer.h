#pragma once
//缓冲opengl派生类
#include <string>
#include "Hazel/Renderer/Buffer.h"
namespace Hazel {
    class OpenGLVertexBuffer :public VertexBuffer {
    public:
        OpenGLVertexBuffer(float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();
        virtual void Bind() const override;//空的虚函数
        virtual void Unbind() const override;

        virtual const BufferLayout& GetLayout() const override {
            return m_Layout;
        }
        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    private:
        uint32_t m_RendererID;//缓冲
        BufferLayout m_Layout;
    };
    class OpenGLIndexBuffer :public IndexBuffer {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();
        virtual void Bind() const;//空的虚函数
        virtual void Unbind() const;
        virtual uint32_t GetCount()const { return m_count; }//返回计数

    private:
        uint32_t m_RendererID;
        uint32_t m_count;
    };
}