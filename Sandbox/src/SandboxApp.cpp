//客户端client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"Sandbox2D.h"//层
#include"ExampleLayer.h"


//client
class Sandbox : public Hazel::Application
{
public:
    Sandbox(const Hazel::ApplicationSpecification& specification)
        : Hazel::Application(specification)
	{
        //pushlayer(new ExampleLayer());//创建示例层
        pushlayer(new Sandbox2D());//2d渲染层
        //pushlayer(new GameLayer());//游戏层
	}
	~Sandbox()
	{

	}
};

//返回客户端
Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) 
{
    //应用规范
    ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "../Hazelnut";
    spec.CommandLineArgs = args;
    //
    return new Sandbox(spec);
}