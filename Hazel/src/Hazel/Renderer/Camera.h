#pragma once
//新的摄像机类
#include <glm/glm.hpp>

namespace Hazel {

    class Camera
    {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection)//接受投影矩阵
            : m_Projection(projection) {}

        virtual ~Camera() = default;

        const glm::mat4& GetProjection() const { return m_Projection; }
    protected:
        glm::mat4 m_Projection = glm::mat4(1.0f);
    };

}