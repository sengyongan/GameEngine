#include<Hazel.h>

//示例层
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
class Sandbox :public Hazel::Application//继承入口点，相当于创建入口点
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
//define
//创建应用程序实例
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}