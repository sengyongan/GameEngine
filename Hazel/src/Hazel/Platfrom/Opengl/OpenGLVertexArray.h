#pragma once
//�����������ݲ���opengl������
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
        virtual void AddVertexBuffer(const Ref< VertexBuffer>& vertexBuffer)  override;
        virtual void SetIndexBuffer(const Ref< IndexBuffer>& indexBuffer)  override;
        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffer() const { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffers; }

    private:
        uint32_t m_RendererID;//�ͻ���ʱһ��
        std::vector<Ref<VertexBuffer>>m_VertexBuffers;//��������
        Ref<IndexBuffer>m_IndexBuffers;//����ָ������
        

    };

}
