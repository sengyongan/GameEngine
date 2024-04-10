#pragma once
//��ɫ�����߼���
#include <string>
#include<unordered_map>
#include<glm/glm.hpp>
namespace Hazel {
    class Shader {
    public:
        virtual ~Shader() = default;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetIntArray(const std::string& name, int* value, uint32_t count) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

        virtual const std::string&  GetName() const = 0;

        static Ref<Shader> Create(const std::string& filepath);//������ɫ��·��
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);//���ܶ��㣬Ƭ����ɫ��
    };
    //��ɫ����---�ӿ�
    class ShaderLibrary {
    public:
        void Add(const Ref<Shader>& shader);//������ɫ�������ܼ�--�ַ������֣����ֵ--��ɫ��
        void Add(const std::string& name, const Ref<Shader>& shader);

        Ref<Shader> Load(const std::string& filePath);//����������ɫ���ļ�������ӵ���ɫ����
        Ref<Shader> Load(const std::string& name, const std::string& filePath);

        Ref<Shader> Get(const std::string& name);//��ȡ�ļ���

        bool Exists(const std::string& name)const;//����ҵ���ɫ����������
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders; //��ɫ���⣺�����ַ������֣�������ɫ��
    };
}