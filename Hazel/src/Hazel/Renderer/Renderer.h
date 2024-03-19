#pragma once
//渲染
//RendererAPI->RendererCommand->Renderer
#include"RendererCommand.h"
#include"OrthgraphicCamera.h"
#include"Shader.h"
namespace Hazel {
    class Renderer {
    public:
        static void BeginScene(OrthgraphicCamera& camera);//开始准备
        static void EndScene();
        static void Submit( const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray,const glm::mat4& transfrom = glm::mat4(1.0f));//渲染

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;//视图矩阵数据
        };
        static SceneData* m_SceneData;
    };
}