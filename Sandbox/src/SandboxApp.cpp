//客户端client
#include<Hazel.h>
#include"imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>

//示例层
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() //投影边界
        :Layer("Example"),m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
    {
        //三角形///////////////////////////////////////////////////////////////////////
        m_VertexArray.reset(Hazel::VertexArray::Create());//智能指针绑定
        float vertices[3 * 7] = {//三角坐标(左下，右下，上）
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        //
        Hazel::BufferLayout layout = {//ButterElements的结构体
                 {Hazel::ShaderDataType::Float3,"a_Position"},//type，name,Normalized
                 {Hazel::ShaderDataType::Float4,"a_Color"}//type，name,Normalized
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = { 0,1,2 };
        std::shared_ptr<Hazel::IndexBuffer>indexBuffer;
        indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        //背景////////////////////////////////////////////////////////////////////
        m_SquareVA.reset(Hazel::VertexArray::Create());//更改所有权
        float squareVertices[3 * 4] = {
              -0.5f, -0.5f, 0.0f,//左下右下，右上左上
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
    )";//第二个着色器——背景
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
    )";//片段着色器
        std::string blueShaderFragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 color;
        in vec3 v_Position;

        void main(){
            color = vec4(0.2,0.2,0.8,1.0);
        }
    )";//传入着色器程序
        m_BlueShader.reset(new Hazel::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));


    }
    void OnUpdate(Hazel::Timestep ts) override {   
        //HZ_CLIENT_TRACE("time:{0}s", ts.GetSeconds());获取时差
        //float ts = ts;隐式强制转换，发生返回m_Time
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


        //初始化窗口
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
                Hazel::Renderer::Submit(m_BlueShader, m_SquareVA, transfrom);//传入到着色器UploadUnifromMat4
            }
        }

        //Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
        
    }
    virtual void OnImGuiRender()override {

    }
    void OnEvent(Hazel::Event& event)override {//目的将其绑定，非判断，比较成立event.Handled |= KeyPressedEvent(static_cast<T&>(m_Event));
        //Hazel::EventDispatcher dispatcher(event);//类对象，从而调用KeyPressedEvent
        ////???Dispatch如何判断的，
        //dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
    }
    //bool OnKeyPressedEvent(Hazel::KeyPressedEvent & event){
    //    return false;
    //}
private:
    //三角形
    std::shared_ptr<Hazel::Shader>m_Shader;//着色器类——指针
    std::shared_ptr<Hazel::VertexArray>m_VertexArray;//顶点数组类
    //背景
    std::shared_ptr<Hazel::Shader>m_BlueShader;//着色器类
    std::shared_ptr<Hazel::VertexArray>m_SquareVA;//顶点数组类

    Hazel::OrthgraphicCamera m_Camera;
    //摄像机变量
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