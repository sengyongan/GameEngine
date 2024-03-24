#include"hzpch.h"
#include "Renderer.h"
#include "Hazel/Platfrom//Opengl/OpenGLShader.h"
namespace Hazel {
    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
    void Renderer::Init()
    {
        RendererCommand::Init();
    }
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RendererCommand::SetViewport(0, 0, width, height);
    }
    void Renderer::BeginScene(OrthgraphicCamera& camera)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjextionMatrix();//°ó¶¨ÎªViewProjextionMatrix
    }
    void Renderer::EndScene()
    {
    }
    void Renderer::Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray, const glm::mat4& transfrom)
    {
        shader->Bind();
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transfrom", transfrom);

        //mi->Bind();

        vertexArray->Bind();
        RendererCommand::DrawIndexed(vertexArray);
    }
}