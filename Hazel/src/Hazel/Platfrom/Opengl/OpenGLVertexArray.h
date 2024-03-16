#pragma once
//顶点数组的重写
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
        virtual void AddVertexBuffer(const std::shared_ptr< VertexBuffer>& vertexBuffer)  override;
        virtual void SetIndexBuffer(const std::shared_ptr< IndexBuffer>& indexBuffer)  override;
        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const { return m_VertexBuffers; }
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffers; }

    private:
        uint32_t m_RendererID;//和缓冲时一样
        std::vector<std::shared_ptr<VertexBuffer>>m_VertexBuffers;//数组类型
        std::shared_ptr<IndexBuffer>m_IndexBuffers;//智能指针类型
        

    };

}
