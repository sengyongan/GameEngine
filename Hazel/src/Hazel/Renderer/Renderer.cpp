#include"hzpch.h"
#include "Renderer.h"
namespace Hazel {
    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
    void Renderer::BeginScene(OrthgraphicCamera& camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjextionMatrix();//°ó¶¨ÎªViewProjextionMatrix
    }
    void Renderer::EndScene()
    {
    }
    void Renderer::Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transfrom)
    {
        shader->Bind();
        shader->UploadUnifromMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        shader->UploadUnifromMat4("u_Transfrom", transfrom);
        vertexArray->Bind();
        RendererCommand::DrawIndexed(vertexArray);
    }
}