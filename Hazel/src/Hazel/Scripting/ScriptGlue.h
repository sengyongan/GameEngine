#pragma once
//定义的c++函数API，供c#调用
namespace Hazel {

    class ScriptGlue
    {
    public:
        //都在脚本引擎调用
        static void RegisterComponents();//注册组件，添加内部调用中
        static void RegisterFunctions();//注册函数,添加内部调用中
    };
}