#pragma once
//用于交互的函数
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

#include <filesystem>
#include <string>

extern "C" {//按照C语言的方式进行编译和链接
    typedef struct _MonoClass MonoClass;//struct _MonoClass定义为新的类型 MonoClass
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
}
namespace Hazel
{    
    //c#中的"Hazel", "Entity"的 子类，底层操作
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className);
        //包含从c++调用c#脚本,的一些操作
        MonoObject* Instantiate();//实例化类
        MonoMethod* GetMethod(const std::string& name, int parameterCount);//获取方法
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);//调用方法
    private:
        std::string m_ClassNamespace;//类的命名空间名
        std::string m_ClassName;//类的类名

        MonoClass* m_MonoClass = nullptr;//类
    };

    //c#脚本实例，调用子类的方法
    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

        void InvokeOnCreate();//本地脚本创建
        void InvokeOnUpdate(float ts);////本地脚本创建更新
    private:
        Ref<ScriptClass> m_ScriptClass;//脚本中的类

        MonoObject* m_Instance = nullptr;///实例对象
        MonoMethod* m_Constructor = nullptr;//构造函数
        MonoMethod* m_OnCreateMethod = nullptr;//创建方法
        MonoMethod* m_OnUpdateMethod = nullptr;//更新方法
    };

    //脚本间访问的核心
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const std::filesystem::path& filepath);

        static void OnRuntimeStart(Scene* scene);//场景
        static void OnRuntimeStop();

        static bool EntityClassExists(const std::string& fullClassName);
        static void OnCreateEntity(Entity entity);//调用子类的启用创建方法
        static void OnUpdateEntity(Entity entity, Timestep ts);

        static Scene* GetSceneContext();
        static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();

        static MonoImage* GetCoreAssemblyImage();//二进制映像
    private:
        static void InitMono();
        static void ShutdownMono();

        static MonoObject* InstantiateClass(MonoClass* monoClass);//本地化类
        static void LoadAssemblyClasses(MonoAssembly* assembly);//加载组件类

        friend class ScriptClass;
        friend class ScriptGlue;
    };

}
