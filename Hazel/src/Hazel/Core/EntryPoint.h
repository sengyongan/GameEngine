#pragma once
//���ܿͻ���
#ifdef HZ_PLATFROM_WINDOWS
//extern���Ա���ģ�������ģ��ʹ�á�
extern Hazel::Application* Hazel::CreatApplication();
//��ڵ�---����Ӧ�ó���
int main(int argc, char** argv)//ָ�루��ַ����ָ�루ֵ��
{
	Hazel::Log::Init();

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");//���������ļ�
    auto app = Hazel::CreatApplication();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
    app->Run();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
    delete app;
    HZ_PROFILE_END_SESSION();
}
#endif