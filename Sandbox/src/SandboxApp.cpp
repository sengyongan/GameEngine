//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"Sandbox2D.h"//��
#include"ExampleLayer.h"
#include "GameLayer.h"


//client
class Sandbox : public Hazel::Application
{
public:
	Sandbox()
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
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}