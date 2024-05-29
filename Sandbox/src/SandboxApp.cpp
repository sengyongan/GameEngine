//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"Sandbox2D.h"//��
#include"ExampleLayer.h"


//client
class Sandbox : public Hazel::Application
{
public:
    Sandbox(const Hazel::ApplicationSpecification& specification)
        : Hazel::Application(specification)
	{
        //pushlayer(new ExampleLayer());//����ʾ����
        pushlayer(new Sandbox2D());//2d��Ⱦ��
        //pushlayer(new GameLayer());//��Ϸ��
	}
	~Sandbox()
	{

	}
};

//���ؿͻ���
Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) 
{
    //Ӧ�ù淶
    ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "../Hazelnut";
    spec.CommandLineArgs = args;
    //
    return new Sandbox(spec);
}