//�ͻ���client
#include<Hazel.h>
#include"imgui/imgui.h"
#include "Hazel/Platfrom//Opengl/OpenGLShader.h"
#include<glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//ʾ����
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() //ͶӰ�߽�
        :Layer("Example"), m_CameraController(1280.0f / 720.0f)
    {
        //������///////////////////////////////////////////////////////////////////////
        m_VertexArray.reset(Hazel::VertexArray::Create());//����ָ���
        float vertices[3 * 7] = {//��������(���£����£��ϣ�
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        //
        Hazel::BufferLayout layout = {//ButterElements�Ľṹ��
                 {Hazel::ShaderDataType::Float3,"a_Position"},//type��name,Normalized
                 {Hazel::ShaderDataType::Float4,"a_Color"}//type��name,Normalized
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0,1,2 };
        Hazel::Ref<Hazel::IndexBuffer>indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        //����////////////////////////////////////////////////////////////////////
        m_SquareVA.reset(Hazel::VertexArray::Create());//��������Ȩ
        float squareVertices[5 * 4] = {//���������ٺ��� 
              -0.5f, -0.5f, 0.0f, 0.0f,0.0f,//�������£���������
               0.5f, -0.5f, 0.0f, 1.0f,0.0f,
               0.5f,  0.5f, 0.0f, 1.0f,1.0f,
              -0.5f,  0.5f, 0.0f, 0.0f,1.0f 
        };
        Hazel::Ref<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({//����BufferLayout���Ͳ���
             {Hazel::ShaderDataType::Float3,"a_Position"},
             {Hazel::ShaderDataType::Float2,"a_TexCood"}
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };
        Hazel::Ref<Hazel::IndexBuffer>squareIB;
        squareIB.reset(Hazel::IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);
        ///////////////////////////////////////////////////////////////////////////
        /*����λ�ã�a_Position��ת��Ϊ��βü��ռ��е����꣨gl_Position����
        ������ɫ������R"()"����ʾ�����ַ�������б��\����Ҫ����ת�壬����ֱ��ʹ��*/
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
    )";//Ƭ����ɫ��
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
        //�ڶ�����ɫ����������
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
    )";//Ƭ����ɫ��
        std::string flatColorShaderFragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 color;
        in vec3 v_Position;
        uniform vec3 u_Color;


        void main(){
            color =vec4(u_Color,1.0f);
        }
    )";//������ɫ������
        m_FlatColorShader = (Hazel::Shader::Create("FlatColor",flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

        //������ɫ������
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

        //��ʼ������
        Hazel::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Hazel::RendererCommand::Clear();

        Hazel::Renderer::BeginScene(m_CameraController.GetCamera());
        //�任
        static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        //Hazel::MaterialRef material = new Hazel::Material(m_FlatColorShader);//����
        //Hazel::MaterialInstanceRef material = new Hazel::MaterialInstance(material);//����ʵ��

        //mi->SetValue('u_Color', redColor);//����ʵ��������ɫ
        //mi->SetTeture('u_AIbedoMap', texture);//����ʵ����������
        //squareMesh->SetMaterial(mi);//�������ò���

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();//dynamic_pointer_castת��ΪOpenGLShader
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

        for (int i = 0; i <20; i++) {
            for (int j = 0; j < 20; j++) {
                glm::vec3 pos(j * 0.11f, i * 0.11f, 0.0f);//ijλ�ã��Ȼ���������
                glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), pos) * scale;//����λ��������Ϊ0.1
                Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transfrom);//���뵽��ɫ��UploadUnifromMat4
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
    virtual void OnImGuiRender()override {//��Ⱦ
        ImGui::Begin("Setting");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }
    void OnEvent(Hazel::Event& event)override
    {
        m_CameraController.OnEvent(event);
    }
private:
    Hazel::ShaderLibrary m_ShaderLibrary;//��ɫ����
    //������
    Hazel::Ref<Hazel::Shader>m_Shader;//��ɫ���ࡪ��ָ��
    Hazel::Ref<Hazel::VertexArray>m_VertexArray;//����������
    //����
    Hazel::Ref<Hazel::Shader>m_FlatColorShader ;//��ɫ����
    Hazel::Ref<Hazel::VertexArray>m_SquareVA;//����������
    //����ͼƬ
    Hazel::Ref<Hazel::Texture2D>m_Texture,m_awesomefaceTexture;//����
    //camera
    Hazel::OrthographicCameraController m_CameraController;
    //������ɫ
    glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};
//client
class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
        pushlayer(new ExampleLayer());//����ʾ����
	}
	~Sandbox()
	{

	}
};

//���ؿͻ���
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}