#include"hzpch.h"
#include"Renderer2D.h"
#include"VertexArray.h"
#include"Shader.h"
#include"RenderCommand.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    struct QuadVertex {//一个quad实例的属性
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TextureFactor;
    };

    struct Renderer2DData {//2D数据
        //限制范围
        static const uint32_t MaxQuads = 10000;//最大四边形数量;
        static const uint32_t MaxVertices = MaxQuads * 4;//quad的顶点数量为4个
        static const uint32_t MaxIndices = MaxQuads * 6;//quad的顶点索引数量为6个
        static const uint32_t MaxTextureSlots = 32;//纹理最多个数

        uint32_t QuadIndexCount = 0;//四边形顶点索引数量
        QuadVertex* QuadVertexBufferBase = nullptr;//quad顶点缓冲数组指针
        QuadVertex* QuadVertexBufferPtr = nullptr;////当前quad顶点缓冲指针

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WihleTexture;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSolts;//纹理数组,MaxTextureSlots设置数组大小
        uint32_t TextureSlotIndex = 1;//当前纹理索引，因为0是白色纹理

        glm::vec4 QuadVertexPositions[4];//四个顶点的位置

        Renderer2D::Statistics Stats;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Renderer2DData s_Data;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Renderer2D::Init() {
        HZ_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = VertexArray::Create();//初始化
        //VertexBuffer
        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));//总quad顶点数 * 一个顶点数据的大小
        //Layout
        s_Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2,"a_TexCood"},
            { ShaderDataType::Float,"a_TexIndex"},
            { ShaderDataType::Float,"a_TextureFactor"}
            });
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
        //IndexBuffer
        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];//所有四边形顶点索引的数组

        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)//每个四边形6个索引缓冲，总Quad索引数
        {
            quadIndices[i + 0] = offset + 0;//4个偏移0 1 2 3 
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;//增加4个偏移 
        }

        Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(squareIB);
        delete[] quadIndices;

        //WihleTexture
        s_Data.WihleTexture = Texture2D::Create(1, 1);// 创建纹理类
        uint32_t TextureData = 0xffffffff;//会导致白色或最亮的颜色
        s_Data.WihleTexture->SetData(&TextureData,sizeof(uint32_t));//new m_RendererID设置为白色纹理
        //数组并初始化
        int32_t samplers[s_Data.MaxTextureSlots];
        for (uint32_t i = 0;i < s_Data.MaxTextureSlots;i++) {
            samplers[i] = i;
        }
        //Shader
        s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("u_Texture", samplers,s_Data.MaxTextureSlots);
        //纹理第一个为白色纹理
        s_Data.TextureSolts[0] = s_Data.WihleTexture;
        //顶点位置
        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
 ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Renderer2D::Shutdown(){
        HZ_PROFILE_FUNCTION();
        delete[] s_Data.QuadVertexBufferBase;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        HZ_PROFILE_FUNCTION();

        glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

        StartBatch();
    }

    void Renderer2D::BeginScene(const EditorCamera& camera)
    {
        glm::mat4 viewProj = camera.GetViewProjection();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);
        StartBatch();
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        HZ_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjextionMatrix());

        StartBatch();
    }
    void Renderer2D::StartBatch()
    {
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;//纹理索引
    }

    void Renderer2D::EndScene(){
        HZ_PROFILE_FUNCTION();
        Flush();
    }
    void Renderer2D::Flush()//
    {
        if (s_Data.QuadIndexCount == 0)
            return; // Nothing to draw
        //
        uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;//两个指针之间的字节偏移量。
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//数据上传到当前绑定的数组缓冲区
        //
        for (uint32_t i = 0;i < s_Data.TextureSlotIndex;i++) {//设置纹理绑定的插槽位置
            s_Data.TextureSolts[i]->Bind(i);
        }

        RenderCommand::DrawIndexed(s_Data.QuadVertexArray,s_Data.QuadIndexCount);
        s_Data.Stats.DrawCalls++;
    }
    void Renderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //绘制DrawQuad/DrawRotatedQuad/////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

        DrawQuad(transform, color);
    }

    //绘制
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float TextureFactor, const glm::vec4& color) {
        DrawQuad({ position.x, position.y, 0.0f }, size, texture, TextureFactor, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float TextureFactor , const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

        DrawQuad(transform, texture, TextureFactor, color);

    }
    //////mat4/////////////////////////////////////////////////////////
    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();
        if (s_Data.QuadIndexCount > Renderer2DData::MaxIndices) {//超出范围立即停止
            NextBatch();
        }
        //当前quad的四个顶点的属性，QuadVertexBufferPtr指向QuadVertexBufferBase[QuadVertex（struct）]数组的成员，设置每个结构体成员的属性
        constexpr size_t quadVertexCount = 4;
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float tilingFactor = 1.0f;

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TextureFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr++;
        }


        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;

    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float TextureFactor, const glm::vec4& color)
    {
        if (s_Data.QuadIndexCount > Renderer2DData::MaxIndices) {
            NextBatch();
        }

        //当前quad的四个顶点的属性
        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec4 tintColor = { 1.0f,1.0f,1.0f,1.0f };
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        float textureIndex = 0.0f;//从0开始查找，获取当前纹理texture在TextureSolts 数组中的位置
        for (uint32_t i = 1;i < s_Data.TextureSlotIndex;i++) {//TextureSlotIndex当前数组中的有数据索引的位置
            if (*s_Data.TextureSolts[i].get() == *texture.get()) {
                textureIndex = (float)i;//如果找到了就返回该位置的索引值
                break;
            }
        }
        if (textureIndex == 0.0f) {//表示没有找到
            textureIndex = (float)s_Data.TextureSlotIndex;//textureIndex=数组末尾
            s_Data.TextureSolts[s_Data.TextureSlotIndex] = texture;//并在数组末尾添加texture
            s_Data.TextureSlotIndex++;
        }

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = tintColor;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TextureFactor = TextureFactor;
            s_Data.QuadVertexBufferPtr++;
        }
        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;

    }

    //////DrawRotatedQuad/////////////////////////////////////////////////////////
    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation,color);//position转为3元

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();
        if (s_Data.QuadIndexCount > Renderer2DData::MaxIndices) {
            NextBatch();
        }

        constexpr size_t quadVertexCount = 4;
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float tilingFactor = 1.0f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TextureFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor, const glm::vec4& color)
    {
        DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, texture, TextureFactor, color);

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor, const glm::vec4& color)
    {
        HZ_PROFILE_FUNCTION();
        if (s_Data.QuadIndexCount > Renderer2DData::MaxIndices) {
            NextBatch();
        }

        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        constexpr glm::vec4 tintColor = { 1.0f,1.0f,1.0f,1.0f };

        float textureIndex = 0.0f;//从0开始查找，获取当前纹理texture在TextureSolts 数组中的位置

        for (uint32_t i = 1;i < s_Data.TextureSlotIndex;i++) {//TextureSlotIndex当前数组中的有数据索引的位置
            if (*s_Data.TextureSolts[i].get() == *texture.get()) {
                textureIndex = (float)i;//如果找到了就返回该位置的索引值
                break;
            }
        }
        if (textureIndex == 0.0f) {//表示没有找到
            textureIndex = (float)s_Data.TextureSlotIndex;//textureIndex=数组末尾
            s_Data.TextureSolts[s_Data.TextureSlotIndex] = texture;//并在数组末尾添加texture
            s_Data.TextureSlotIndex++;
        }
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f),rotation, { 0.0f,0.0f,1.0f })
            * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = tintColor;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TextureFactor = TextureFactor;
            s_Data.QuadVertexBufferPtr++;
        }


        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }


}