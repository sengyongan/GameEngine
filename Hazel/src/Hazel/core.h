#pragma once
//宏
#include <memory>
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
//
//#ifdef HZ_DEBUG
//#define HZ_ENABLE_ASSERTS
//#endif
//
#ifdef HZ_ENABLE_ASSERTS
//如果启用宏--检查特定条件
//进行判断
//断言（如果x不存在）那么_debugbreak中断调试
    #define HZ_ASSERT(x,...){if(!(x)){HZ_CLIENT_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
    #define HZ_CORE_ASSERT(x,...){if(!(x)){HZ_CORE_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
#else//不用判断
    #define HZ_ASSERT(x,...)
    #define HZ_CORE_ASSERT(x,...)
#endif

//事件的类型标签
#define BIT(x) (1 <<x)//把1左移一位

//bind—————第一个参数表示对象的成员函数（可调用对象），第二个参数表示对象的地址,fn形参 = 占位符对应的实参
#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)//fn 绑定到当前对象（this），调用宏即调用fn函数

//指针别名
namespace Hazel {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}