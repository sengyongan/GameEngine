#pragma once
//�����dll
//�Զ�ִ��Ԥ�������
#ifdef HZ_PLATFROM_WINDOWS//�����windowsƽ̨
	#ifdef HZ_BUILD_DLL//��ǰ�Ƿ���dll��Ŀ��
	#define HAZEL_API _declspec(dllexport)//�����Ķ�̬���ӿ⺯��
	#else
	#define HAZEL_API _declspec(dllimport)//Ԥ���岻�����ӵ�����
	#endif
#else//r���������ƽ̨
#error Hazel only support Windows!
#endif