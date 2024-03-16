#include"hzpch.h"
#include"Shader.h"
#include<glad/glad.h>
namespace Hazel {
    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        // Create an empty vertex shader handle
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);//typedef unsigned int GLuint;

        // 将顶点着色器的源代码发送给 GL
      // 注意 std：：string 的 c_str 以 NULL 字符结束。
        const GLchar* source = vertexSrc.c_str();//typedef char GLchar;//GLchar为char别名
        glShaderSource(vertexShader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(vertexShader);
        //检查是否成功
        GLint isCompiled = 0;//GL_COMPILE_STATUS编译状态
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);//获取isCompiled
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;//GL_INFO_LOG_LENGTH日志长度
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(vertexShader);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Vertex shader compilation failure!");
            return;
        }

        // Create an empty fragment shader handle
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        //将片段着色器源代码发送给GL
        //注意std::string的。c_str是NULL字符终止。
        source = fragmentSrc.c_str();
        glShaderSource(fragmentShader, 1, &source, 0);

        // Compile the fragment shader
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragmentShader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertexShader);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Fragment shader compilation failure!");
            return;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        m_RendererID = glCreateProgram();
        GLuint program = m_RendererID;

        // Attach our shaders to our program
        glAttachShader(program, vertexShader);//添加到程序
        glAttachShader(program, fragmentShader);

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
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Shader link failure!");
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);//删除着色器
    }
    Shader::~Shader()
    {
        glDeleteProgram(m_RendererID);//销毁着色器程序
    }
    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }
    void Shader::Unbind() const
    {
        glUseProgram(0);
    }
}