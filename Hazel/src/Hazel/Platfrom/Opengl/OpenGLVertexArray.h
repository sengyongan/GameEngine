#pragma once
//�����������д
#include"Hazel/Renderer/VertexArray.h"
namespace Hazel {
    class OpenGLVertexArray:public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();
        virtual void Bind() const override;//�յ��麯��
        virtual void Unbind() const override;
        //��ӻ���
        virtual void AddVertexBuffer(const std::shared_ptr< VertexBuffer>& vertexBuffer)  override;
        virtual void SetIndexBuffer(const std::shared_ptr< IndexBuffer>& indexBuffer)  override;
        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const { return m_VertexBuffers; }
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffers; }

    private:
        uint32_t m_RendererID;//�ͻ���ʱһ��
        std::vector<std::shared_ptr<VertexBuffer>>m_VertexBuffers;//��������
        std::shared_ptr<IndexBuffer>m_IndexBuffers;//����ָ������
        

    };

}
