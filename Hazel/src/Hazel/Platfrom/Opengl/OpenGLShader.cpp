//��ɫ������---��ȡ�ļ�
#include"hzpch.h"
#include"OpenGLShader.h"
#include<fstream>
#include<glad/glad.h> 
#include<glm/gtc/type_ptr.hpp>
namespace Hazel {
    static GLenum ShaderTypeFromString (const std::string& type) {
        if (type == "vertex")//����
            return GL_VERTEX_SHADER;//glCreateShader(GL_VERTEX_SHADER)
        if (type == "fragment" || type == "pixel")//Ƭ��/����
            return GL_FRAGMENT_SHADER;//glCreateShader(GL_FRAGMENT_SHADER)

        HZ_CORE_ASSERT(false, "Unknown shader type!");//����֪����
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filepath) {
        HZ_PROFILE_FUNCTION();

        std::string source = ReadFile(filepath);//��ȡ��ɫ����Դ��
        auto shaderSources = PreProcess(source);//��Դ����
        Compile(shaderSources);//������ɫ��
        //��ȡpath���ļ���
        //assets/shaders/Texture.glsl
        auto lastSlach = filepath.find_last_of("/\\");
        lastSlach = lastSlach == std::string::npos ? 0 : lastSlach + 1;//û���ҵ�
        auto lastDot = filepath.rfind(".");
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlach : lastDot - lastSlach;//����Texture����
        m_Name = filepath.substr(lastSlach, count);

    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)//ͬʱ֧��
        :m_Name(name)
    {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);//Ҳ���Ե���
    }
    OpenGLShader::~OpenGLShader()
    {
        HZ_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);//������ɫ������
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

        //��һ���ļ��ж�ȡ���ݲ�����洢��һ����Ϊresult��������
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);//��һ���ļ�,����ģʽ | ������ģʽ
        if (in) {
            in.seekg(0, std::ios::end);//�ļ�ָ���ƶ����ļ�ĩβ���Ա��ȡ�ļ��Ĵ�С
            result.resize(in.tellg());//��ȡ�ļ��Ĵ�С������result�����Ĵ�С����Ϊ�ô�С��
            in.seekg(0, std::ios::beg);//���ļ�ָ�������ƶ����ļ���ͷ���Ա��ͷ��ʼ��ȡ���ݡ�
            in.read(&result[0], result.size());//���ļ��ж�ȡ���ݣ�������洢��result������
            in.close();//�ر��ļ�����
        }
        else {
            HZ_CORE_ERROR("Could not open file '{0}'", filepath);
        }
        return result;
    }
    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        HZ_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;//�洢---ö�����ͣ�vertex����)����Ӧ��Դ��

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);//��ȡ"#type"�ĳ���
        size_t pos = source.find(typeToken, 0);//���� "#type"λ��
        while (pos != std::string::npos)//ֱ���Ҳ���"#type"
        {
            size_t eol = source.find_first_of("\r\n", pos);//�ҵ���ǰ "#type"λ�õ��н�����
            HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");

            size_t begin = pos + typeTokenLength + 1;//�����ַ�������ʼλ�ã���ĩβ - ������ʼλ���ų�"#type"�ĳ��ȣ�
            std::string type = source.substr(begin, eol - begin);//��ȡtype���ַ�
            HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);//��һ���ǿ��е���ʼλ�ã���ɫ��Դ�����ʼ��
            pos = source.find(typeToken, nextLinePos);//��һ��"#type"λ��
            //enum--ShaderTypeFromString    ��nposδ�ҵ�ָ���Ӵ����ַ��������pos - nextLinePos
            shaderSources[ShaderTypeFromString(type)] = 
                source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }
        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        HZ_PROFILE_FUNCTION();

        //��ɫ������
        GLuint program = glCreateProgram();
        HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");//��֧��������ɫ��
        std::array<GLenum,2> glShaderIDs;//��ɫ������
        int glShaderIDIndex = 0;//��ɫ������
        //������ɫ��Դ��unordered_map
        for (auto& kv : shaderSources) {
            GLenum type = kv.first;//��ȡ��ɫ�� Դ�� ����
            const std::string& source = kv.second;//��ȡ��ɫ�� Դ��
            GLuint shader = glCreateShader(type);//������ɫ��
            //
            // ��������ɫ����Դ���뷢�͸� GL
            // ע�� std����string �� c_str �� NULL �ַ�������
            const GLchar* sourceSrc = source.c_str();//typedef char GLchar;//GLcharΪchar����
            glShaderSource(shader, 1, &sourceSrc, 0);

            // Compile the vertex shader
            glCompileShader(shader);
            //����Ƿ�ɹ�
            GLint isCompiled = 0;//GL_COMPILE_STATUS����״̬
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);//��ȡisCompiled
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;//GL_INFO_LOG_LENGTH��־����
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
            glAttachShader(program, shader);//��ӵ�����
            glShaderIDs[glShaderIDIndex++] = shader; 
        }
        m_RendererID = program;
        // Link our program
        glLinkProgram(program);//����

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;//���Ӽ��
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