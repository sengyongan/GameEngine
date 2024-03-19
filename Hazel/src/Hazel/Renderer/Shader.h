#pragma once
//着色器（逻辑）
#include<glm/glm.hpp>
#include <string>
namespace Hazel {
    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();
        void Bind() const;
        void Unbind() const;

        void UploadUnifromMat4(const std::string& name, const glm::mat4& matrix);//将视图投影矩阵传递给顶点着色器u_ViewProjection
    private:
        uint32_t m_RendererID;//所有着色器,references在shader.cpp72
    };
}