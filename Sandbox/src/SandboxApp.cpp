//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"Sandbox2D.h"//��
#include"ExampleLayer.h"


//client
class Sandbox : public Hazel::Application
{
public:
	Sandbox(Hazel::ApplicationCommandLineArgs args)
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
    return new Sandbox(args);
}