#pragma once
//��Ⱦ
//RendererAPI->RendererCommand->Renderer
#include"RendererCommand.h"
#include"OrthgraphicCamera.h"
#include"Shader.h"
namespace Hazel {
    class Renderer {
    public:
        static void BeginScene(OrthgraphicCamera& camera);//��ʼ׼��
        static void EndScene();
        static void Submit( const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray,const glm::mat4& transfrom = glm::mat4(1.0f));//��Ⱦ

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;//��ͼ��������
        };
        static SceneData* m_SceneData;
    };
}