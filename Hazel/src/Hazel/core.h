#pragma once
//定义宏dll
//自动执行预定义操作
#ifdef HZ_PLATFROM_WINDOWS//如果在windows平台
	#ifdef HZ_BUILD_DLL//当前是否在dll项目中
	#define HAZEL_API _declspec(dllexport)//导出的动态链接库函数
	#else
	#define HAZEL_API _declspec(dllimport)//预定义不用链接到库中
	#endif
#else//r如果在其他平台
#error Hazel only support Windows!
#endif