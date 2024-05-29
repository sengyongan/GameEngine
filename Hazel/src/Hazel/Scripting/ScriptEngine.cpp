#include "hzpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include "ScriptGlue.h"

namespace Hazel {
    namespace Utils {
        char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)//返回文件内容的指针，将文件大小传递给outSize
        {
            std::ifstream stream(filepath, std::ios::binary | std::ios::ate);//创建流，以二进制模式和文件末尾位置打开文件

            if (!stream)
            {
                // Failed to open the file
                return nullptr;
            }

            std::streampos end = stream.tellg();//获取文件末尾的位置，将其存储在end变量中
            stream.seekg(0, std::ios::beg);//将文件指针移回文件开头
            uint32_t size = end - stream.tellg();//计算文件大小，即end - 当前文件指针位置

            if (size == 0)//文件为空
            {
                // File is empty
                return nullptr;
            }

            char* buffer = new char[size];//字符数组buffer，大小为文件大小
            stream.read((char*)buffer, size);//将文件内容读取到buffer中
            stream.close();//关闭文件流

            *outSize = size;//大小赋值给outSize指针所指向的变量
            return buffer;//返回包含文件内容的buffer
        }
        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)//从路径加载程序集
        {
            uint32_t fileSize = 0;//用于存储文件大小
            char* fileData = ReadBytes(assemblyPath, &fileSize);//从路径读取到字符数组，并将大小传入fileData中

            // NOTE: 除了加载程序集之外，我们无法使用该图像，因为该图像没有程序集引用
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);//传入文件数据、文件大小、标志位（这里为1）、状态指针和用户数据

            if (status != MONO_IMAGE_OK)//打开图像时发生错误
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }
            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);//加载程序集
            mono_image_close(image);//关闭图像对象image

            // Don't forget to free the file data
            delete[] fileData;

            return assembly;//返回程序集
        }

        void PrintAssemblyTypes(MonoAssembly* assembly)//传入程序集，打印程序类型
        {
            MonoImage* image = mono_assembly_get_image(assembly);//获取图像对象image
            const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);//获取类型表
            int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);//获取表中的行数

            for (int32_t i = 0; i < numTypes; i++)//遍历所有类型
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];//数组
                mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);//第i行数据解码到cols数组中

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);//名称空间
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);//名称对应的字符串

                HZ_CORE_TRACE("{}.{}", nameSpace, name);
            }
        }

    }
    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;//根域指针
        MonoDomain* AppDomain = nullptr;//创建应用程序域

        MonoAssembly* CoreAssembly = nullptr;//c#程序域
        MonoImage* CoreAssemblyImage = nullptr;//核心组件图像

        ScriptClass EntityClass;//c#程序域中类
    };

    static ScriptEngineData* s_Data = nullptr;

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();
        LoadAssembly("Resources/Scripts/Hazel-ScriptCore.dll");
        
        ScriptGlue::RegisterFunctions();

        //获取c#程序域中名为"Hazel"的命名空间和名为"Main"的类
        s_Data->EntityClass = ScriptClass("Hazel", "Entity");

        // 1. 创建mono的class对象
        MonoObject* instance = s_Data->EntityClass.Instantiate();

        // 2. 调用功能
        MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);//找到method
        s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);//调用函数，params为传入的参数

        // 3. 调用带参数的函数
        MonoMethod* printIntFunc = s_Data->EntityClass.GetMethod("PrintInt", 1);

        int value = 5;
        void* param = &value;//参数

        s_Data->EntityClass.InvokeMethod(instance, printIntFunc, &param);
        //
        MonoMethod* printIntsFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);//传入参数数量=2
        int value2 = 508;
        void* params[2] =
        {
            &value,
            &value2
        };
        s_Data->EntityClass.InvokeMethod(instance, printIntsFunc, params);
        //
        MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");//Mono字符串对象
        MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
        void* stringParam = monoString;
        s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);
    }

    void ScriptEngine::ShutdownMono()
    {
        // NOTE(Yan): mono is a little confusing to shutdown, so maybe come back to this

        // mono_domain_unload(s_Data->AppDomain);
        s_Data->AppDomain = nullptr;

        // mono_jit_cleanup(s_Data->RootDomain);
        s_Data->RootDomain = nullptr;
    }

    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)//加载应用程序域
    {
        //创建应用程序域
        s_Data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
        mono_domain_set(s_Data->AppDomain, true);//设置为当前域

        //获取c#程序域
        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);//获取image
        // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
    }
    void ScriptEngine::InitMono()//调用c#程序域的函数
    {
        mono_set_assemblies_path("mono/lib");

        MonoDomain* rootDomain = mono_jit_init("HazelJITRuntime");//创建一个根域
        HZ_CORE_ASSERT(rootDomain);

        //存储根域指针到s_Data
        s_Data->RootDomain = rootDomain;
    }

    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete s_Data;
    }
    
    MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)//实例化类
    {
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        mono_runtime_object_init(instance);//（并调用构造函数）
        return instance;
    }

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
    }

    MonoObject* ScriptClass::Instantiate()//实例化类
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)//获取c#类中的方法
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)//调用方法
    {
        return mono_runtime_invoke(method, instance, params, nullptr);
    }
}