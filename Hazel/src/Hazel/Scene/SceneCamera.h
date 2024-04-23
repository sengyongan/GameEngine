//场景摄像机（由imgui面板控制位置）
#pragma once
#include"Hazel/Renderer/Camera.h"

namespace Hazel {
    class SceneCamera : public Camera {
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthgraphic(float size, float nearClip, float farClip);//设置正交变量

        void SetViewportSize(uint32_t width, uint32_t height);//设置视口纵横比

        float GetOrthopraghicSize() const { return m_OrthographicSize; }//获取摄像机缩放
        void SetOrthopraghicSize(float size) {
            m_OrthographicSize = size; RecalculateProject();
        }
    private:
        void RecalculateProject();//重新计算投影矩阵
    private:
        float m_OrthographicSize = 10.0f;//缩放
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;//远近平面

        float m_AspectRatio = 0.0f;//纵横比
    };
}