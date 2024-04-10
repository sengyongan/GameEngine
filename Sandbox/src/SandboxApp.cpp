//客户端client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"Sandbox2D.h"//层
#include"ExampleLayer.h"
#include "GameLayer.h"


//client
class Sandbox : public Hazel::Application
{
public:
	Sandbox()
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
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}