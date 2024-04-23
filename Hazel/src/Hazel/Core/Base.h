#pragma once
//��
#include <memory>
// ʹ��Ԥ�������ƽ̨
#ifdef _WIN32
    /* Windows x64/x86 */
    #ifdef _WIN64
        /* Windows x64  */
        #define HZ_PLATFORM_WINDOWS
    #else
        /* Windows x86 ����֧�� x86 �汾��*/
       #error "x86 Builds are not supported!" 
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    /* TARGET_OS_MAC ����������ƽ̨�ϡ�
     * �������Ǳ���������ƽ̨������˳��
     * ȷ���������� MAC ������
     * ����������ƻ��ƽ̨ */
     /*��֧�ֲ���ϵͳģ����
      ��֧�� IOS��
      ��֧�� MacOS
      δ֪��ƻ��ƽ̨
     */
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define HZ_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define HZ_PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else
        #error "Unknown Apple platform!"
    #endif
 /* ���ǻ������� __linux__ ֮ǰ��� __ANDROID__
  * ��Ϊ��׿�ǻ��� linux �ں˵�
  * ���Ѿ�������__linux__ */
    /*
    ��֧�ְ�׿ϵͳ��
    ��֧�� Linux��
    */
#elif defined(__ANDROID__)
    #define HZ_PLATFORM_ANDROID
    #error "Android is not supported!"
#elif defined(__linux__)
    #define HZ_PLATFORM_LINUX
    #error "Linux is not supported!"
#else
    /* Unknown compiler/platform */
    #error "Unknown platform!"
#endif // ƽ̨ĩ�˼��


#ifdef HZ_DEBUG
    #define HZ_ENABLE_ASSERTS
#endif


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

//����
#ifdef HZ_ENABLE_ASSERTS

//�����x�����ڣ���ô_debugbreak�жϵ���
    #define HZ_ASSERT(x,...){if(!(x)){HZ_CLIENT_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
    #define HZ_CORE_ASSERT(x,...){if(!(x)){HZ_CORE_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
#else//�����ж�
    #define HZ_ASSERT(x,...)
    #define HZ_CORE_ASSERT(x,...)
#endif

//�¼������ͱ�ǩ
#define BIT(x) (1 <<x)//��1����һλ

//bind������������һ��������ʾ����ĳ�Ա�������ɵ��ö��󣩣��ڶ���������ʾ����ĵ�ַ,fn�β� = ռλ����Ӧ��ʵ��
//#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)
//event������������ת����OnEvent��Ա����
#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
//ָ�����
namespace Hazel {
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T,typename ...Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ...Args>
    constexpr Scope<T> CreateRef(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

}