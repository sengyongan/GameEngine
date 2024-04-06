#include"hzpch.h"
#include"Renderer2D.h"
#include"VertexArray.h"
#include"Shader.h"
#include"RenderCommand.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {
    struct Renderer2DStorage {//2D存储
        Ref<VertexArray> QuadVertexArray;//定义+声明
        Ref<Shader> TextureShader;
        Ref<Texture2D> Texture;
    };
    static Renderer2DStorage* s_Data;

    void Renderer2D::Init() {
        HZ_PROFILE_FUNCTION();

        s_Data = new Renderer2DStorage();
        s_Data->QuadVertexArray = VertexArray::Create();//初始化

        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f,0.0f,0.0f,
             0.5f, -0.5f, 0.0f,1.0f,0.0f,
             0.5f,  0.5f, 0.0f,1.0f,1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f,1.0f
        };

        Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2,"a_TexCood"}
            });
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

        s_Data->Texture = Texture2D::Create(1, 1);// 创建纹理类
        uint32_t TextureData = 0xffffffff;//会导致白色或最亮的颜色
        s_Data->Texture->SetData(&TextureData,sizeof(uint32_t));//new m_RendererID设置为白色纹理

        s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");

        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", 0);

    }
    void Renderer2D::Shutdown(){
        HZ_PROFILE_FUNCTION();

        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        HZ_PROFILE_FUNCTION();

        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjextionMatrix());
    }
    void Renderer2D::EndScene(){
        HZ_PROFILE_FUNCTION();

    }
    //绘制
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->Texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        s_Data->TextureShader->SetMat4("u_Transform", transform);
        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
    /////Temp----GameLayer/////////////////////////////////////////////////////
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
    //绘制
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, rotation, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->Texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        s_Data->TextureShader->SetMat4("u_Transform", transform);
        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
    ///Temp----GameLayer/////////////////////////////////////////////////////
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
    {
        s_Data->TextureShader->SetFloat4("u_Color", color);
        texture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }

    //////DrawRotatedQuad/////////////////////////////////////////////////////////
    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation,color);//position转为3元

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();

        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->TextureShader->SetFloat("u_TextureFactor", 1.0f);

        s_Data->Texture->Bind();

        glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), position) 
            * glm::rotate(glm::mat4(1.0f),rotation, { 0.0f,0.0f,1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
        s_Data->TextureShader->SetMat4("u_Transfrom", transfrom);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor, const glm::vec4& color)
    {
        DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, texture, TextureFactor, color);

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();

        s_Data->TextureShader->SetFloat4("u_Color", color);
        s_Data->TextureShader->SetFloat("u_TextureFactor", TextureFactor);

        s_Data->TextureShader->Bind();

        glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), position) 
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
        s_Data->TextureShader->SetMat4("u_Transfrom", transfrom);

        texture->Bind();

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);

    }


}