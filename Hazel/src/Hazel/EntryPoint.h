#pragma once
//接受客户端
#ifdef HZ_PLATFROM_WINDOWS
//extern可以被本模块或其他模块使用。
extern Hazel::Application* Hazel::CreatApplication();
//入口点---创建应用程序
int main(int argc, char** argv)//指针（地址）的指针（值）
{
	Hazel::Log::Init();
	HZ_CORE_WARN("log");
	HZ_CLIENT_INFO("Hello");

	auto app=Hazel::CreatApplication();
	app->Run();
	delete app;
}
#endif