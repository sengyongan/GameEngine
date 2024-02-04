#include<Hazel.h>

//ʾ����
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() :Layer("Example")
    {}
    void OnUpdate() override
    {
        HZ_CLIENT_INFO("ExampleLayer: :Update");
    }
    void OnEvent(Hazel::Event& event)override
    {
        HZ_CLIENT_TRACE("{0}", event);
    }
};

//client
class Sandbox :public Hazel::Application//�̳���ڵ㣬�൱�ڴ�����ڵ�
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
//define
//����Ӧ�ó���ʵ��
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}