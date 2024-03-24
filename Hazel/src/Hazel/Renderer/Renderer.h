#pragma once
//��Ⱦ��API
//RendererAPI->RendererCommand->Renderer
#include"RendererCommand.h"
#include"OrthgraphicCamera.h"
#include"Shader.h"
namespace Hazel {
    class Renderer {
    public:
        static void Init();//��Ⱦ��ʼ��

        static void OnWindowResize(uint32_t width,uint32_t height);//���ڵ�����С

        static void BeginScene(OrthgraphicCamera& camera);//��ʼ׼��
        static void EndScene();
        static void Submit( const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transfrom = glm::mat4(1.0f));//��Ⱦ

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;//��ͼ��������
        };
        static SceneData* m_SceneData;
    };
}