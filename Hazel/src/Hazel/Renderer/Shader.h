#pragma once
//着色器（逻辑）
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

        static Ref<Shader> Create(const std::string& filepath);//接受着色器路径
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);//接受顶点，片段着色器
    };
    //着色器库---接口
    class ShaderLibrary {
    public:
        void Add(const Ref<Shader>& shader);//传入着色器：接受键--字符串名字，添加值--着色器
        void Add(const std::string& name, const Ref<Shader>& shader);

        Ref<Shader> Load(const std::string& filePath);//解析编译着色器文件，并添加到着色器库
        Ref<Shader> Load(const std::string& name, const std::string& filePath);

        Ref<Shader> Get(const std::string& name);//获取文件名

        bool Exists(const std::string& name)const;//如果找到着色器，返回真
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders; //着色器库：接受字符串名字，返回着色器
    };
}