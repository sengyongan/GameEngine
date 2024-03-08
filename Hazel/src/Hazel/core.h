#pragma once
//定义宏--包含在premack5.lua
#ifdef HZ_PLATFROM_WINDOWS//windows平台
	#ifdef HZ_BUILD_DLL//是否构建dll项目
//预定义宏的功能
	#define HAZEL_API _declspec(dllexport)//导出的动态链接库函数
	#else
	#define HAZEL_API _declspec(dllimport)//预定义不用链接到库中
	#endif
#else//r如果在其他平台
    #error Hazel only support Windows!
#endif

//
#ifdef HZ_ENABLE_ASSERTS//如果启用宏--检查特定条件
//进行判断
//宏起到断言作用（如果括号内不存在）_debugbreak中断调试
    #define HZ_ASSERT(x,...){if(!(x)){HZ_CLIENT_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
    #define HZ_CORE_ASSERT(x,...){if(!(x)){HZ_CORE_ERROR("Assertion Faild:{0}",__VA_ARGS__);__debugbreak();}}
#else//不用判断
    #define HZ_ASSERT(x,...)
    #define HZ_CORE_ASSERT(x,...)
#endif

//事件的类型标签
#define BIT(x) (1 <<x)//把1左移一位

//this指向在当前类使用
#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)
