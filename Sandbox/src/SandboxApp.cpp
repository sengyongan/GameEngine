//客户端client
#include<Hazel.h>
#include"imgui/imgui.h"
#include "Hazel/Platfrom//Opengl/OpenGLShader.h"
#include<glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//示例层
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() //投影边界
        :Layer("Example"), m_CameraController(1280.0f / 720.0f)
    {
        //三角形///////////////////////////////////////////////////////////////////////
        m_VertexArray.reset(Hazel::VertexArray::Create());//智能指针绑定
        float vertices[3 * 7] = {//三角坐标(左下，右下，上）
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        //
        Hazel::BufferLayout layout = {//ButterElements的结构体
                 {Hazel::ShaderDataType::Float3,"a_Position"},//type，name,Normalized
                 {Hazel::ShaderDataType::Float4,"a_Color"}//type，name,Normalized
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0,1,2 };
        Hazel::Ref<Hazel::IndexBuffer>indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        //背景////////////////////////////////////////////////////////////////////
        m_SquareVA.reset(Hazel::VertexArray::Create());//更改所有权
        float squareVertices[5 * 4] = {//先列数，再横数 
              -0.5f, -0.5f, 0.0f, 0.0f,0.0f,//左下右下，右上左上
               0.5f, -0.5f, 0.0f, 1.0f,0.0f,
               0.5f,  0.5f, 0.0f, 1.0f,1.0f,
              -0.5f,  0.5f, 0.0f, 0.0f,1.0f 
        };
        Hazel::Ref<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({//接受BufferLayout类型参数
             {Hazel::ShaderDataType::Float3,"a_Position"},
             {Hazel::ShaderDataType::Float2,"a_TexCood"}
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };
        Hazel::Ref<Hazel::IndexBuffer>squareIB;
        squareIB.reset(Hazel::IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);
        ///////////////////////////////////////////////////////////////////////////
        /*顶点位置（a_Position）转换为齐次裁剪空间中的坐标（gl_Position）。
        顶点着色器——R"()"来表示多行字符串，反斜杠\不需要进行转义，可以直接使用*/
        std::string vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transfrom;
        out vec3 v_Position;
        out vec4 v_Color;
        void main(){
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_ViewProjection * u_Transfrom * vec4(a_Position,1.0);
        }
    )";//片段着色器
        std::string fragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 color;
        in vec3 v_Position;
        in vec4 v_Color;

        void main(){
            color = vec4(v_Position*0.5+0.5,1.0);
            color = v_Color;
        }
    )";
        m_Shader = (Hazel::Shader::Create("VertexPosColor",vertexSrc, fragmentSrc));
        //第二个着色器——背景
        std::string flatColorShaderVertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transfrom;

        out vec3 v_Position;
        void main(){
            v_Position = a_Position;
            gl_Position = u_ViewProjection * u_Transfrom * vec4(a_Position,1.0);
        }
    )";//片段着色器
        std::string flatColorShaderFragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 color;
        in vec3 v_Position;
        uniform vec3 u_Color;


        void main(){
            color =vec4(u_Color,1.0f);
        }
    )";//传入着色器程序
        m_FlatColorShader = (Hazel::Shader::Create("FlatColor",flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

        //传入着色器程序
        auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
        m_Texture = Hazel::Texture2D::Create("assets/textures/container.png");
        m_awesomefaceTexture = Hazel::Texture2D::Create("assets/textures/awesomeface.png");

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

    }
    void OnUpdate(Hazel::Timestep ts) override
    {   
        //camera
        m_CameraController.OnUpdate(ts);

        //初始化窗口
        Hazel::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Hazel::RendererCommand::Clear();

        Hazel::Renderer::BeginScene(m_CameraController.GetCamera());
        //变换
        static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        //Hazel::MaterialRef material = new Hazel::Material(m_FlatColorShader);//材质
        //Hazel::MaterialInstanceRef material = new Hazel::MaterialInstance(material);//材质实例

        //mi->SetValue('u_Color', redColor);//材质实例设置颜色
        //mi->SetTeture('u_AIbedoMap', texture);//材质实例设置纹理
        //squareMesh->SetMaterial(mi);//网格设置材质

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();//dynamic_pointer_cast转换为OpenGLShader
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

        for (int i = 0; i <20; i++) {
            for (int j = 0; j < 20; j++) {
                glm::vec3 pos(j * 0.11f, i * 0.11f, 0.0f);//ij位置，先绘制行再列
                glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), pos) * scale;//先移位，后缩放为0.1
                Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transfrom);//传入到着色器UploadUnifromMat4
            }
        }

        auto textureShader = m_ShaderLibrary.Get("Texture");

        m_Texture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_awesomefaceTexture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
        
    }
    virtual void OnImGuiRender()override {//渲染
        ImGui::Begin("Setting");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }
    void OnEvent(Hazel::Event& event)override
    {
        m_CameraController.OnEvent(event);
    }
private:
    Hazel::ShaderLibrary m_ShaderLibrary;//着色器库
    //三角形
    Hazel::Ref<Hazel::Shader>m_Shader;//着色器类——指针
    Hazel::Ref<Hazel::VertexArray>m_VertexArray;//顶点数组类
    //背景
    Hazel::Ref<Hazel::Shader>m_FlatColorShader ;//着色器类
    Hazel::Ref<Hazel::VertexArray>m_SquareVA;//顶点数组类
    //纹理图片
    Hazel::Ref<Hazel::Texture2D>m_Texture,m_awesomefaceTexture;//纹理
    //camera
    Hazel::OrthographicCameraController m_CameraController;
    //背景颜色
    glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};
//client
class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
        pushlayer(new ExampleLayer());//创建示例层
	}
	~Sandbox()
	{

	}
};

//返回客户端
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}