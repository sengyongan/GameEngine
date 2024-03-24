#pragma once
#include"Hazel/Renderer/Shader.h"
//#include<glad/glad.h>  Cannot open incude file:'glad/gladhs 
#include<glm/glm.hpp>
//TODO
typedef unsigned int GLenum;
namespace Hazel {
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();
        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual const std::string& GetName() const override { return m_Name; }

        //name��ɫ���е�ȫ�ֳ���unifrom����value/matrix���ݵ�ֵ
        void UploadUniformInt(const std::string& name, int value);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix); //����ͼͶӰ���󴫵ݸ�������ɫ��u_ViewProjection
    private:
        std::string ReadFile(const std::string& filepath);//��ȡ�ļ�
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);//���ַ���Դ��ָ���
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);//gl�ײ�Դ��--forѭ��unordered_map
    private:
        uint32_t m_RendererID;//��ɫ������
        std::string m_Name;//������ļ���  / ֱ�Ӵ����ʵ�Σ� ��OpenGLShader���캯���б���ʼ��
    };
}