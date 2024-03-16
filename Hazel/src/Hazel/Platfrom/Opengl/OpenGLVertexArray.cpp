#include "hzpch.h"
#include "OpenGLVertexArray.h"
#include<glad/glad.h>
namespace Hazel {
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)//获取类型——传入BufferLayout缓冲布局
    {
        switch (type)
        {
        case Hazel::ShaderDataType::Float:    return GL_FLOAT;
        case Hazel::ShaderDataType::Float2:   return GL_FLOAT;
        case Hazel::ShaderDataType::Float3:   return GL_FLOAT;
        case Hazel::ShaderDataType::Float4:   return GL_FLOAT;
        case Hazel::ShaderDataType::Mat3:     return GL_FLOAT;
        case Hazel::ShaderDataType::Mat4:     return GL_FLOAT;
        case Hazel::ShaderDataType::Int:      return GL_INT;
        case Hazel::ShaderDataType::Int2:     return GL_INT;
        case Hazel::ShaderDataType::Int3:     return GL_INT;
        case Hazel::ShaderDataType::Int4:     return GL_INT;
        case Hazel::ShaderDataType::Bool:     return GL_BOOL;
        }

        HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }
    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }
    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }
    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }
    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) 
    {
        HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "vertex buffer has no layout!");
        glBindVertexArray(m_RendererID);//绑定顶点数组
        vertexBuffer->Bind();
        uint32_t index = 0;//索引位置在这里定义
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offest);
            index++;
        }
        m_VertexBuffers.push_back(vertexBuffer);

    }
    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) 
    {
        glBindVertexArray(m_RendererID);//绑定顶点数组
        indexBuffer->Bind();
        m_IndexBuffers = indexBuffer;
    }
}