#pragma once
#ifdef HZ_PLATFROM_WINDOWS
//extern可以被本模块或其他模块使用。
extern Hazel::Application* Hazel::CreatApplication();
//入口点---创建应用程序
int main(int argc, char** argv)//指针（地址）的指针（值）
{
	printf("Hazel Engine\n");
	auto app=Hazel::CreatApplication();
	app->Run();
	delete app;
}
#endif