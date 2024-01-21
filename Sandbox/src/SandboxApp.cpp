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
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}