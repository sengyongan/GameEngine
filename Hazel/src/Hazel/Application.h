#pragma once
#include "core.h"
namespace Hazel {
	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();//����run����
	};
	//to be define in client
	Application* CreatApplication();

}