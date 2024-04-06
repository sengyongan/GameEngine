//着色器程序---读取文件
#include"hzpch.h"
#include"OpenGLShader.h"
#include<fstream>
#include<glad/glad.h> 
#include<glm/gtc/type_ptr.hpp>
namespace Hazel {
    static GLenum ShaderTypeFromString (const std::string& type) {
        if (type == "vertex")//顶点
            return GL_VERTEX_SHADER;//glCreateShader(GL_VERTEX_SHADER)
        if (type == "fragment" || type == "pixel")//片段/像素
            return GL_FRAGMENT_SHADER;//glCreateShader(GL_FRAGMENT_SHADER)

        HZ_CORE_ASSERT(false, "Unknown shader type!");//非已知类型
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filepath) {
        HZ_PROFILE_FUNCTION();

        std::string source = ReadFile(filepath);//获取着色器的源码
        auto shaderSources = PreProcess(source);//将源码拆分
        Compile(shaderSources);//编译着色器
        //获取path中文件名
        //assets/shaders/Texture.glsl
        auto lastSlach = filepath.find_last_of("/\\");
        lastSlach = lastSlach == std::string::npos ? 0 : lastSlach + 1;//没有找到
        auto lastDot = filepath.rfind(".");
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlach : lastDot - lastSlach;//计算Texture长度
        m_Name = filepath.substr(lastSlach, count);

    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)//同时支持
        :m_Name(name)
    {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);//也可以调用
    }
    OpenGLShader::~OpenGLShader()
    {
        HZ_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);//销毁着色器程序
    }
    void OpenGLShader::Bind() const
    {
        HZ_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }
    void OpenGLShader::Unbind() const
    {
        HZ_PROFILE_FUNCTION();

        glUseProgram(0);
    }

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        HZ_PROFILE_FUNCTION();

        UploadUniformInt(name, value);
    }

    void OpenGLShader::SetFloat(const std::string& name, float value)
    {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat(name, value);

    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat3(name, value);
    }
    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
        HZ_PROFILE_FUNCTION();

        UploadUniformFloat4(name, value);
    }
    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
        HZ_PROFILE_FUNCTION();

        UploadUniformMat4(name, value);
    }


    void OpenGLShader::UploadUniformInt(const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    std::string OpenGLShader::ReadFile(const std::string& filepath)
    {
        HZ_PROFILE_FUNCTION();

        //从一个文件中读取数据并将其存储到一个名为result的容器中
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);//打开一个文件,输入模式 | 二进制模式
        if (in) {
            in.seekg(0, std::ios::end);//文件指针移动到文件末尾，以便获取文件的大小
            result.resize(in.tellg());//获取文件的大小，并将result容器的大小调整为该大小。
            in.seekg(0, std::ios::beg);//将文件指针重新移动到文件开头，以便从头开始读取数据。
            in.read(&result[0], result.size());//从文件中读取数据，并将其存储到result容器中
            in.close();//关闭文件流。
        }
        else {
            HZ_CORE_ERROR("Could not open file '{0}'", filepath);
        }
        return result;
    }
    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;//存储---枚举类型（vertex……)，对应的源码

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);//获取"#type"的长度
        size_t pos = source.find(typeToken, 0);//查找 "#type"位置
        while (pos != std::string::npos)//直到找不到"#type"
        {
            size_t eol = source.find_first_of("\r\n", pos);//找到当前 "#type"位置的行结束符
            HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");

            size_t begin = pos + typeTokenLength + 1;//计算字符串的起始位置，行末尾 - （行起始位置排除"#type"的长度）
            std::string type = source.substr(begin, eol - begin);//提取type的字符
            HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);//下一个非空行的起始位置（着色器源码的起始）
            pos = source.find(typeToken, nextLinePos);//下一个"#type"位置
            //enum--ShaderTypeFromString    ，npos未找到指定子串或字符的情况，pos - nextLinePos
            shaderSources[ShaderTypeFromString(type)] = 
                source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }
        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        HZ_PROFILE_FUNCTION();

        //着色器程序
        GLuint program = glCreateProgram();
        HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");//仅支持两个着色器
        std::array<GLenum,2> glShaderIDs;//着色器个数
        int glShaderIDIndex = 0;//着色器索引
        //迭代着色器源码unordered_map
        for (auto& kv : shaderSources) {
            GLenum type = kv.first;//获取着色器 源码 类型
            const std::string& source = kv.second;//获取着色器 源码
            GLuint shader = glCreateShader(type);//创建着色器
            //
            // 将顶点着色器的源代码发送给 GL
            // 注意 std：：string 的 c_str 以 NULL 字符结束。
            const GLchar* sourceSrc = source.c_str();//typedef char GLchar;//GLchar为char别名
            glShaderSource(shader, 1, &sourceSrc, 0);

            // Compile the vertex shader
            glCompileShader(shader);
            //检查是否成功
            GLint isCompiled = 0;//GL_COMPILE_STATUS编译状态
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);//获取isCompiled
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;//GL_INFO_LOG_LENGTH日志长度
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                // We don't need the shader anymore.
                glDeleteShader(shader);

                HZ_CORE_ERROR("{0}", infoLog.data());
                HZ_CORE_ASSERT(false, "shader compilation failure!");
                break;
            }
            glAttachShader(program, shader);//添加到程序
            glShaderIDs[glShaderIDIndex++] = shader; 
        }
        m_RendererID = program;
        // Link our program
        glLinkProgram(program);//链接

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;//链接检查
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            for (auto id : glShaderIDs) {
                glDeleteShader(id);
            }

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "OpenGLShader link failure!");
            return;
        }

        // Always detach shaders after a successful link.
        for (auto id : glShaderIDs) {
            glDetachShader(program, id);
        }
    }

}