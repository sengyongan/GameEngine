#include"hzpch.h"
#include"OpenGLBuffer.h"
#include<glad/glad.h>
namespace Hazel {
    ///////////////////////////////////////////////////////////////////////////
    //vetexbuffer//////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)//包含3个
    {   //glad_glCreateBuffers(Glsizei n, GLuint *buffers);
        HZ_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);//
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);//传递字节大小
    }
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    {
        HZ_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);//
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);//传递字节大小

    }
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        HZ_PROFILE_FUNCTION();

        glDeleteBuffers(1, &m_RendererID);
    }
    void OpenGLVertexBuffer::Bind() const
    {
        HZ_PROFILE_FUNCTION();

        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);//只包含glBindBuffer，有时只需要这一个
    }
    void OpenGLVertexBuffer::Unbind() const
    {
        HZ_PROFILE_FUNCTION();

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);//缓冲区对象m_RendererID绑定到当前的数组缓冲区目标GL_ARRAY_BUFFER
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);//数据上传到当前绑定的数组缓冲区中，覆盖原有的数据。
    }
    ///////////////////////////////////////////////////////////////////////////
    //indexbuffer//////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
        :m_count(count)
    {
        HZ_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);//传递个数
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count* sizeof(uint32_t), indices, GL_STATIC_DRAW);

    }
    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        HZ_PROFILE_FUNCTION();

        glDeleteBuffers(1, &m_RendererID);
    }
    void OpenGLIndexBuffer::Bind() const
    {
        HZ_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLIndexBuffer::Unbind() const
    {
        HZ_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    }
}