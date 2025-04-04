#include"hzpch.h"
#include"OpenGLRendererAPI.h"
#include<glad/glad.h>
namespace Hazel {
    void OpenGLRendererAPI::Init()
    {
        HZ_PROFILE_FUNCTION();

        glEnable(GL_BLEND);//���û��
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//���û�Ϻ���
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);//Line
    }
    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }
    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }
    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//��ɫ���壬��Ȼ���
    }
    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();
        uint32_t count = indexCount ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        //glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        vertexArray->Bind();
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        glLineWidth(width);
    }
}