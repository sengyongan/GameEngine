#pragma once
//解析顶点数据布局opengl派生类
#include"Hazel/Renderer/VertexArray.h"
namespace Hazel {
    class OpenGLVertexArray:public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();
        virtual void Bind() const override;//空的虚函数
        virtual void Unbind() const override;
        //添加缓冲
        virtual void AddVertexBuffer(const Ref< VertexBuffer>& vertexBuffer)  override;
        virtual void SetIndexBuffer(const Ref< IndexBuffer>& indexBuffer)  override;
        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffer() const { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffers; }

    private:
        uint32_t m_RendererID;//和缓冲时一样
        std::vector<Ref<VertexBuffer>>m_VertexBuffers;//数组类型
        Ref<IndexBuffer>m_IndexBuffers;//智能指针类型
        

    };

}
