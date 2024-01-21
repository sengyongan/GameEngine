#pragma once
//∂®“Â∫Ídll
#ifdef HZ_PLATFROM_WINDOWS
	#ifdef HZ_BUILD_DLL
	#define HAZEL_API _declspec(dllexport)
	#else
	#define HAZEL_API _declspec(dllexport)
	#endif
#else
#error Hazel only support Windows!
#endif