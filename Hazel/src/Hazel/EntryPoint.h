#pragma once
#ifdef HZ_PLATFROM_WINDOWS
//extern���Ա���ģ�������ģ��ʹ�á�
extern Hazel::Application* Hazel::CreatApplication();
//��ڵ�---����Ӧ�ó���
int main(int argc, char** argv)//ָ�루��ַ����ָ�루ֵ��
{
	printf("Hazel Engine\n");
	auto app=Hazel::CreatApplication();
	app->Run();
	delete app;
}
#endif