#pragma once
//��ɫ�����߼���
#include<glm/glm.hpp>
#include <string>
namespace Hazel {
    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();
        void Bind() const;
        void Unbind() const;

        void UploadUnifromMat4(const std::string& name, const glm::mat4& matrix);//����ͼͶӰ���󴫵ݸ�������ɫ��u_ViewProjection
    private:
        uint32_t m_RendererID;//������ɫ��,references��shader.cpp72
    };
}