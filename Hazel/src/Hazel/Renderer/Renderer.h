#pragma once
//渲染器API
//RendererAPI->RendererCommand->Renderer
#include"RendererCommand.h"
#include"OrthgraphicCamera.h"
#include"Shader.h"
namespace Hazel {
    class Renderer {
    public:
        static void Init();//渲染初始化

        static void OnWindowResize(uint32_t width,uint32_t height);//窗口调整大小

        static void BeginScene(OrthgraphicCamera& camera);//开始准备
        static void EndScene();
        static void Submit( const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transfrom = glm::mat4(1.0f));//渲染

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;//视图矩阵数据
        };
        static SceneData* m_SceneData;
    };
}