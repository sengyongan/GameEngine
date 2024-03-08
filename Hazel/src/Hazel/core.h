#pragma once
//�����--������premack5.lua
#ifdef HZ_PLATFROM_WINDOWS//windowsƽ̨
	#ifdef HZ_BUILD_DLL//�Ƿ񹹽�dll��Ŀ
//Ԥ�����Ĺ���
	#define HAZEL_API _declspec(dllexport)//�����Ķ�̬���ӿ⺯��
	#else
	#define HAZEL_API _declspec(dllimport)//Ԥ���岻�����ӵ�����
	#endif
#else//r���������ƽ̨
    #error Hazel only support Windows!
#endif

//
#ifdef HZ_ENABLE_ASSERTS//������ú�--����ض�����
//�����ж�
//���𵽶������ã���������ڲ����ڣ�_debugbreak�жϵ���
    #define HZ_ASSERT(x,...){if(!(x)){HZ_CLIENT_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
    #define HZ_CORE_ASSERT(x,...){if(!(x)){HZ_CORE_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
#else//�����ж�
    #define HZ_ASSERT(x,...)
    #define HZ_CORE_ASSERT(x,...)
#endif

//�¼������ͱ�ǩ
#define BIT(x) (1 <<x)//��1����һλ

//thisָ���ڵ�ǰ��ʹ��
#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)
