#pragma once
//着色器程序
#include <string>
namespace Hazel {
    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();
        void Bind() const;
        void Unbind() const;
    private:
        uint32_t m_RendererID;//所有着色器
    };
}