#pragma once
#include "core.h"
namespace Hazel {
	class HAZEL_API Application//主要入口点
	{
	public:
		Application();
		virtual ~Application();
		void Run();//声明run函数
	};
	//to be define in client
	Application* CreatApplication();//创建应用程序实例

}