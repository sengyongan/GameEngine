#pragma once
//宏
#include <memory>
#include "Hazel/Core/PlatformDetection.h"
#include "Hazel/Core/Log.h"
#include "Hazel/Core/Assert.h"

#ifdef HZ_DEBUG
#if defined(HZ_PLATFORM_WINDOWS)
#define HZ_DEBUGBREAK() __debugbreak()
#elif defined(HZ_PLATFORM_LINUX)
#include <signal.h>
#define HZ_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define HZ_ENABLE_ASSERTS
#else
#define HZ_DEBUGBREAK()
#endif

#define HZ_EXPAND_MACRO(x) x
#define HZ_STRINGIFY_MACRO(x) #x


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