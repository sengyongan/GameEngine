#pragma once
//���ܿͻ���
#ifdef HZ_PLATFROM_WINDOWS
//extern���Ա���ģ�������ģ��ʹ�á�
extern Hazel::Application* Hazel::CreatApplication();
//��ڵ�---����Ӧ�ó���
int main(int argc, char** argv)//ָ�루��ַ����ָ�루ֵ��
{
	Hazel::Log::Init();
	HZ_CORE_WARN("log");
	HZ_CLIENT_INFO("Hello");

	auto app=Hazel::CreatApplication();
	app->Run();
	delete app;
}
#endif