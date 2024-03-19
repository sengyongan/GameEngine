//�ͻ���client
#include<Hazel.h>
#include"imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>

//ʾ����
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() //ͶӰ�߽�
        :Layer("Example"),m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
    {
        //������///////////////////////////////////////////////////////////////////////
        m_VertexArray.reset(Hazel::VertexArray::Create());//����ָ���
        float vertices[3 * 7] = {//��������(���£����£��ϣ�
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        //
        Hazel::BufferLayout layout = {//ButterElements�Ľṹ��
                 {Hazel::ShaderDataType::Float3,"a_Position"},//type��name,Normalized
                 {Hazel::ShaderDataType::Float4,"a_Color"}//type��name,Normalized
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0,1,2 };
        std::shared_ptr<Hazel::IndexBuffer>indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        //����////////////////////////////////////////////////////////////////////
        m_SquareVA.reset(Hazel::VertexArray::Create());//��������Ȩ
        float squareVertices[3 * 4] = {
              -0.5f, -0.5f, 0.0f,//�������£���������
               0.5f, -0.5f, 0.0f,
               0.5f,  0.5f, 0.0f,
              -0.5f,  0.5f, 0.0f
        };
        std::shared_ptr<Hazel::VertexBuffer> squareVB;
        squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
             {Hazel::ShaderDataType::Float3,"a_Position"},
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<Hazel::IndexBuffer>squareIB;
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
    )";//�ڶ�����ɫ����������
        m_Shader.reset(new Hazel::Shader(vertexSrc, fragmentSrc));
        std::string blueShaderVertexSrc = R"(
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
        std::string blueShaderFragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 color;
        in vec3 v_Position;

        void main(){
            color = vec4(0.2,0.2,0.8,1.0);
        }
    )";//������ɫ������
        m_BlueShader.reset(new Hazel::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));


    }
    void OnUpdate(Hazel::Timestep ts) override {   
        //HZ_CLIENT_TRACE("time:{0}s", ts.GetSeconds());��ȡʱ��
        //float ts = ts;��ʽǿ��ת������������m_Time
        if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed * ts;
        if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
            m_CameraPosition.y += m_CameraMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;
        if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        if (Hazel::Input::IsKeyPressed(HZ_KEY_J))
            m_SquarePosition.x -= m_SquareMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_L))
            m_SquarePosition.x += m_SquareMoveSpeed * ts;
        if (Hazel::Input::IsKeyPressed(HZ_KEY_I))
            m_SquarePosition.y += m_SquareMoveSpeed * ts;
        else if (Hazel::Input::IsKeyPressed(HZ_KEY_K))
            m_SquarePosition.y -= m_SquareMoveSpeed * ts;


        //��ʼ������
        Hazel::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Hazel::RendererCommand::Clear();

        m_Camera.SetRotation(m_CameraRotation);
        m_Camera.SetPosition(m_CameraPosition);

        Hazel::Renderer::BeginScene(m_Camera);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        for (int i = 0; i <20; i++) {
            for (int j = 0; j < 20; j++) {
                glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
                glm::mat4 transfrom = glm::translate(glm::mat4(1.0f), pos) * scale;
                Hazel::Renderer::Submit(m_BlueShader, m_SquareVA, transfrom);//���뵽��ɫ��UploadUnifromMat4
            }
        }

        //Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
        
    }
    virtual void OnImGuiRender()override {

    }
    void OnEvent(Hazel::Event& event)override {//Ŀ�Ľ���󶨣����жϣ��Ƚϳ���event.Handled |= KeyPressedEvent(static_cast<T&>(m_Event));
        //Hazel::EventDispatcher dispatcher(event);//����󣬴Ӷ�����KeyPressedEvent
        ////???Dispatch����жϵģ�
        //dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
    }
    //bool OnKeyPressedEvent(Hazel::KeyPressedEvent & event){
    //    return false;
    //}
private:
    //������
    std::shared_ptr<Hazel::Shader>m_Shader;//��ɫ���ࡪ��ָ��
    std::shared_ptr<Hazel::VertexArray>m_VertexArray;//����������
    //����
    std::shared_ptr<Hazel::Shader>m_BlueShader;//��ɫ����
    std::shared_ptr<Hazel::VertexArray>m_SquareVA;//����������

    Hazel::OrthgraphicCamera m_Camera;
    //���������
    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 5.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 180.0f;

    glm::vec3 m_SquarePosition;
    float m_SquareMoveSpeed = 5.0f;

};
//client
class Sandbox :public Hazel::Application
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