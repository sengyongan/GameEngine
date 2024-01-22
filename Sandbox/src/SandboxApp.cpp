#include<Hazel.h>
//client
class Sandbox :public Hazel::Application
{
public:
	Sandbox()
	{

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