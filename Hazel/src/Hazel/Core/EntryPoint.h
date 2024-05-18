#pragma once
//接受客户端
#include "Hazel/Core/Application.h"
#ifdef HZ_PLATFROM_WINDOWS
//extern可以被本模块或其他模块使用。
extern Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args);
//入口点---创建应用程序
int main(int argc, char** argv)//指针（地址）的指针（值）
{
	Hazel::Log::Init();

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");//生成配置文件
    auto app = Hazel::CreateApplication({ argc, argv });
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
    app->Run();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
    delete app;
    HZ_PROFILE_END_SESSION();
}
#endif