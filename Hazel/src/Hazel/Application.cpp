#include"hzpch.h"
#include "Application.h"
#include"Hazel/Input.h"
#include"Window.h"
#include<glad/glad.h>
namespace Hazel {
    //目前绑定OnEvent函数参数为——placeholders占位符的类模板（未知）
#define BIND_ENENT_FN(x)  std::bind(&Application::x, this, std::placeholders::_1)//绑定&Application::x事件

     Application* Application::s_Instance = nullptr;//类静态成员s_Instance 初始为null
    //在sandboxapp创建类对象
Application::Application()
{
    HZ_CORE_ASSERT(s_Instance, "Application already exists");
    s_Instance = this;
    //指针=window类型，调用函数的指针
    m_Window =std::unique_ptr<Window>(Window::Create());//创建窗口
    m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//调用SetEventCallback就是调用OnEvent事件设置参数
    m_ImGuiLayer = new imGuiLayer;
    pushOverlayer(m_ImGuiLayer);
    //unsigned int id;//无符号整数变量
    //glGenVertexArrays(1, &id);//顶点数组对象
    //三角形///////////////////////////////////////////////////////////////////////
    m_VertexArray.reset(VertexArray::Create());//智能指针绑定
    float vertices[3 * 7] = {//三角坐标(左下，右下，上）
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };
    std::shared_ptr<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
    //
    BufferLayout layout = {//ButterElements的结构体
             {ShaderDataType::Float3,"a_Position"},//type，name,Normalized
             {ShaderDataType::Float4,"a_Color"}//type，name,Normalized
    };
    vertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vertexBuffer);

    uint32_t indices[3] = { 0,1,2 };
    std::shared_ptr<IndexBuffer>indexBuffer;
    indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));  
    m_VertexArray->SetIndexBuffer(indexBuffer);
    //背景////////////////////////////////////////////////////////////////////
    m_SquareVA.reset(VertexArray::Create());//更改所有权
    float squareVertices[3 * 4] = {
                -0.75f, -0.75f, 0.0f,
                 0.75f, -0.75f, 0.0f,
                 0.75f,  0.75f, 0.0f,
                -0.75f,  0.75f, 0.0f
    };
    std::shared_ptr<VertexBuffer> squareVB;
    squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout ( {
         {ShaderDataType::Float3,"a_Position"},
    });
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };
    std::shared_ptr<IndexBuffer>squareIB;
    squareIB.reset(IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(uint32_t)));
    m_SquareVA->SetIndexBuffer(squareIB);
    ///////////////////////////////////////////////////////////////////////////
    /*顶点位置（a_Position）转换为齐次裁剪空间中的坐标（gl_Position）。
    顶点着色器——R"()"来表示多行字符串，反斜杠\不需要进行转义，可以直接使用*/
    std::string vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;
        out vec3 v_Position;
        out vec4 v_Color;
        void main(){
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = vec4(a_Position,1.0);
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
    m_Shader.reset(new Shader(vertexSrc,fragmentSrc));
    std::string blueShaderVertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        out vec3 v_Position;
        void main(){
            v_Position = a_Position;
            gl_Position = vec4(a_Position,1.0);
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
    m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));

}
Application::~Application()
{
}

void Application::pushlayer(Layer* layer)
{
    m_Layerstack.PushLayer(layer);
    layer->OnAttach();
}

void Application::pushOverlayer(Layer* overlay)
{
    m_Layerstack.PushOverlay(overlay);
    overlay->OnAttach();

}

void Application::OnEvent(Event& e)//检查
{
    EventDispatcher dispatcher(e);//调度员
    dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose方法与模板WindowCloseEvent比较
    //层
    for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
    {
        (*--it)->OnEvent(e);//逐个向前遍历--onevent是layer类型（可悬停查看）
        if (e.Handled)////层被处理过
            break;

    }
}


	void Application::Run()
	{
        while (m_Running)
        {   //初始化窗口
            glClearColor(0.1f, 0.1f, 0.1f, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            //先绘制方形
            m_BlueShader->Bind();
            m_SquareVA->Bind();
            glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);//绘制三角形
            //
            m_Shader->Bind();//在绘制前绑定
            m_VertexArray->Bind();//绑定顶点数组
            //(GLenum mode, GLsizei count, GLenum type, const void *indices);
            glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);//绘制三角形
            //
            for (Layer* layer : m_Layerstack)//遍历所有层
                layer->OnUpdate();//调用层实时更新
            //
            m_ImGuiLayer->Begin();
            for (Layer* layer : m_Layerstack)
                layer->OnImGuiRender();//渲染
            m_ImGuiLayer->End();
            //
            m_Window->OnUpdate();//窗口实时更新

        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//窗口关闭
    {
        m_Running = false;//结束运行
        return true;
    }


}