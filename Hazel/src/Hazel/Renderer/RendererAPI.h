#pragma once
//��ͬƽ̨����Ⱦ����ӿ�
//��Ⱦ�Ĳ�ͬͼ��API����ײ㣬RendererAPI->RenderCommand->Renderer��RendererAPI->OpenGLRendererAPI
#include"VertexArray.h"
#include<glm/glm.hpp>
namespace Hazel {
    class RendererAPI {
    public:
        enum class API {//ö����
            None = 0, OpenGL = 1
        };
    public:
        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color)=0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)=0;
        virtual void Clear() = 0;
        //Draw
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray,uint32_t indexCount = 0)=0;//��Submit����
        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;//��render2D����

        virtual void SetLineWidth(float width) = 0;

        inline static API GetAPI() { return s_API;}
    private:
        static API s_API;
    };
} 