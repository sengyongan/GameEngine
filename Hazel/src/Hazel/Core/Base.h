#pragma once
//宏
#include <memory>
// 使用预定义宏检测平台
#ifdef _WIN32
    /* Windows x64/x86 */
    #ifdef _WIN64
        /* Windows x64  */
        #define HZ_PLATFORM_WINDOWS
    #else
        /* Windows x86 ，不支持 x86 版本！*/
       #error "x86 Builds are not supported!" 
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    /* TARGET_OS_MAC 存在于所有平台上。
     * 所以我们必须检查所有平台（按此顺序）
     * 确保我们是在 MAC 上运行
     * 而不是其他苹果平台 */
     /*不支持操作系统模拟器
      不支持 IOS！
      不支持 MacOS
      未知的苹果平台
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
 /* 我们还必须在 __linux__ 之前检查 __ANDROID__
  * 因为安卓是基于 linux 内核的
  * 它已经定义了__linux__ */
    /*
    不支持安卓系统！
    不支持 Linux！
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
#endif // 平台末端检测


#ifdef HZ_DEBUG
    #define HZ_ENABLE_ASSERTS
#endif


//HZ_PLATFROM_WINDOWS--包含在premack5.lua
#ifdef HZ_PLATFROM_WINDOWS//windows平台
    #if HZ_DYNAMIC_LINK//动态链接
        #ifdef HZ_BUILD_DLL//构建dll项目
	        #define HAZEL_API _declspec(dllexport)//导出
	    #else
	        #define HAZEL_API _declspec(dllimport)//导入
        #endif
    #else
        #define HAZEL_API
    #endif
#else//其他平台
    #error Hazel only support Windows!
#endif

//断言
#ifdef HZ_ENABLE_ASSERTS

//（如果x不存在）那么_debugbreak中断调试
    #define HZ_ASSERT(x,...){if(!(x)){HZ_CLIENT_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
    #define HZ_CORE_ASSERT(x,...){if(!(x)){HZ_CORE_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
#else//不用判断
    #define HZ_ASSERT(x,...)
    #define HZ_CORE_ASSERT(x,...)
#endif

//事件的类型标签
#define BIT(x) (1 <<x)//把1左移一位

//bind—————第一个参数表示对象的成员函数（可调用对象），第二个参数表示对象的地址,fn形参 = 占位符对应的实参
//#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)
//event参数将被完美转发给OnEvent成员函数
#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
//指针别名
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