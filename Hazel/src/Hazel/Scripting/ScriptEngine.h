#pragma once
//c#脚本
#include <filesystem>
#include <string>

extern "C" {//按照C语言的方式进行编译和链接
    typedef struct _MonoClass MonoClass;//struct _MonoClass定义为新的类型 MonoClass
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
}
namespace Hazel {

    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const std::filesystem::path& filepath);
    private:
        static void InitMono();
        static void ShutdownMono();

        static MonoObject* InstantiateClass(MonoClass* monoClass);//实例化类

        friend class ScriptClass;
    };
    //对c#脚本的一些操作，封装
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className);

        MonoObject* Instantiate();//实例化类
        MonoMethod* GetMethod(const std::string& name, int parameterCount);//获取方法
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);//调用方法
    private:
        std::string m_ClassNamespace;
        std::string m_ClassName;

        MonoClass* m_MonoClass = nullptr;
    };
}
