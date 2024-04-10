#pragma once
//2D渲染器API
#include"OrthographicCamera.h"
#include"Texture.h"
namespace Hazel {
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();
        static void Flush();//绘制

        //绘制
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        //rotation,处理弧度非角度
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        
        //stats
        struct Statistics {//统计数据
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() { return QuadCount * 4; };
            uint32_t GetTotalIndexCount() { return QuadCount * 6; };
        };

        static void ResetStats();//重置统计,因为绘制发生在update上
        static Statistics GetStats();

    private:
        static void FlushAndReset();
    };
}