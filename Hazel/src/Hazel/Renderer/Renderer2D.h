#pragma once
//2D��Ⱦ��API
#include"OrthographicCamera.h"
#include"Texture.h"
#include"SubTexture2D.h"
#include"Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Font.h"

namespace Hazel {
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();
        static void Flush();//����

        //���ƽӿ�
        //quad
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        //mat4
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

        //rotation,�����ȷǽǶ�
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float TextureFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
        
        //circle&&Line&&Rect����(thickness���,fade��ɫ)
        static void DrawCircle(const glm::mat4 & transform, const glm::vec4 & color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
        static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);//������
        static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        //eitytiID
        static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);
        //String
        static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const glm::vec4& color);
        
        //
        static float GetLineWidth();
        static void SetLineWidth(float width);

        //stats
        struct Statistics {//ͳ������
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() { return QuadCount * 4; };
            uint32_t GetTotalIndexCount() { return QuadCount * 6; };
        };

        static void ResetStats();//����ͳ��,��Ϊ���Ʒ�����update��
        static Statistics GetStats();

    private:
        static void StartBatch();
        static void NextBatch();
    };
}