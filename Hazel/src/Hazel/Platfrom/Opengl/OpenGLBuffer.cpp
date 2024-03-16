#include"hzpch.h"
#include"OpenGLBuffer.h"
#include<glad/glad.h>
namespace Hazel {
    ///////////////////////////////////////////////////////////////////////////
    //vetexbuffer//////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
    {   //glad_glCreateBuffers(Glsizei n, GLuint *buffers);
        glCreateBuffers(1, &m_RendererID);//ºê
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);//Ë¢ÐÂ»º³å
    }
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        glDeleteBuffers(1, &m_RendererID);
    }
    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLVertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    ///////////////////////////////////////////////////////////////////////////
    //indexbuffer//////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
        :m_count(count)
    {   
        glCreateBuffers(1, &m_RendererID);//ºê
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count* sizeof(uint32_t), indices, GL_STATIC_DRAW);//Ë¢ÐÂ»º³å

    }
    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        glDeleteBuffers(1, &m_RendererID);
    }
    void OpenGLIndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLIndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    }
}