#include"hzpch.h"
#include "Application.h"
#include"Hazel/Input.h"
#include"Window.h"
#include<glad/glad.h>
namespace Hazel {
    //Ŀǰ��OnEvent��������Ϊ����placeholdersռλ������ģ�壨δ֪��
#define BIND_ENENT_FN(x)  std::bind(&Application::x, this, std::placeholders::_1)//��&Application::x�¼�

     Application* Application::s_Instance = nullptr;//�ྲ̬��Աs_Instance ��ʼΪnull
    //��sandboxapp���������
Application::Application()
{
    HZ_CORE_ASSERT(s_Instance, "Application already exists");
    s_Instance = this;
    //ָ��=window���ͣ����ú�����ָ��
    m_Window =std::unique_ptr<Window>(Window::Create());//��������
    m_Window->SetEventCallback(BIND_ENENT_FN(OnEvent));//����SetEventCallback���ǵ���OnEvent�¼����ò���
    m_ImGuiLayer = new imGuiLayer;
    pushOverlayer(m_ImGuiLayer);
    //unsigned int id;//�޷�����������
    //glGenVertexArrays(1, &id);//�����������
    //������///////////////////////////////////////////////////////////////////////
    m_VertexArray.reset(VertexArray::Create());//����ָ���
    float vertices[3 * 7] = {//��������(���£����£��ϣ�
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };
    std::shared_ptr<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
    //
    BufferLayout layout = {//ButterElements�Ľṹ��
             {ShaderDataType::Float3,"a_Position"},//type��name,Normalized
             {ShaderDataType::Float4,"a_Color"}//type��name,Normalized
    };
    vertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vertexBuffer);

    uint32_t indices[3] = { 0,1,2 };
    std::shared_ptr<IndexBuffer>indexBuffer;
    indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));  
    m_VertexArray->SetIndexBuffer(indexBuffer);
    //����////////////////////////////////////////////////////////////////////
    m_SquareVA.reset(VertexArray::Create());//��������Ȩ
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
    /*����λ�ã�a_Position��ת��Ϊ��βü��ռ��е����꣨gl_Position����
    ������ɫ������R"()"����ʾ�����ַ�������б��\����Ҫ����ת�壬����ֱ��ʹ��*/
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
    m_Shader.reset(new Shader(vertexSrc,fragmentSrc));
    std::string blueShaderVertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        out vec3 v_Position;
        void main(){
            v_Position = a_Position;
            gl_Position = vec4(a_Position,1.0);
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

void Application::OnEvent(Event& e)//���
{
    EventDispatcher dispatcher(e);//����Ա
    dispatcher.Dispatch <WindowCloseEvent>(BIND_ENENT_FN(OnWindowClose));//OnWindowClose������ģ��WindowCloseEvent�Ƚ�
    //��
    for (auto it = m_Layerstack.end(); it != m_Layerstack.begin();)
    {
        (*--it)->OnEvent(e);//�����ǰ����--onevent��layer���ͣ�����ͣ�鿴��
        if (e.Handled)////�㱻�����
            break;

    }
}


	void Application::Run()
	{
        while (m_Running)
        {   //��ʼ������
            glClearColor(0.1f, 0.1f, 0.1f, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            //�Ȼ��Ʒ���
            m_BlueShader->Bind();
            m_SquareVA->Bind();
            glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);//����������
            //
            m_Shader->Bind();//�ڻ���ǰ��
            m_VertexArray->Bind();//�󶨶�������
            //(GLenum mode, GLsizei count, GLenum type, const void *indices);
            glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);//����������
            //
            for (Layer* layer : m_Layerstack)//�������в�
                layer->OnUpdate();//���ò�ʵʱ����
            //
            m_ImGuiLayer->Begin();
            for (Layer* layer : m_Layerstack)
                layer->OnImGuiRender();//��Ⱦ
            m_ImGuiLayer->End();
            //
            m_Window->OnUpdate();//����ʵʱ����

        }
	}
    bool  Application::OnWindowClose(WindowCloseEvent& e)//���ڹر�
    {
        m_Running = false;//��������
        return true;
    }


}