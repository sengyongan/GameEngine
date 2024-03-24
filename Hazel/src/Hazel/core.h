#pragma once
//��
#include <memory>
//HZ_PLATFROM_WINDOWS--������premack5.lua
#ifdef HZ_PLATFROM_WINDOWS//windowsƽ̨
    #if HZ_DYNAMIC_LINK//��̬����
        #ifdef HZ_BUILD_DLL//����dll��Ŀ
	        #define HAZEL_API _declspec(dllexport)//����
	    #else
	        #define HAZEL_API _declspec(dllimport)//����
        #endif
    #else
        #define HAZEL_API
    #endif
#else//����ƽ̨
    #error Hazel only support Windows!
#endif
//
//#ifdef HZ_DEBUG
//#define HZ_ENABLE_ASSERTS
//#endif
//
#ifdef HZ_ENABLE_ASSERTS
//������ú�--����ض�����
//�����ж�
//���ԣ����x�����ڣ���ô_debugbreak�жϵ���
    #define HZ_ASSERT(x,...){if(!(x)){HZ_CLIENT_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
    #define HZ_CORE_ASSERT(x,...){if(!(x)){HZ_CORE_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
#else//�����ж�
    #define HZ_ASSERT(x,...)
    #define HZ_CORE_ASSERT(x,...)
#endif

//�¼������ͱ�ǩ
#define BIT(x) (1 <<x)//��1����һλ

//bind������������һ��������ʾ����ĳ�Ա�������ɵ��ö��󣩣��ڶ���������ʾ����ĵ�ַ,fn�β� = ռλ����Ӧ��ʵ��
#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)//fn �󶨵���ǰ����this�������ú꼴����fn����

//ָ�����
namespace Hazel {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}