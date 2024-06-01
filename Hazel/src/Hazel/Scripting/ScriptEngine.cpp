#include "hzpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"

#include "ScriptGlue.h"

namespace Hazel {
    //脚本字段映射
    static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
    {
        { "System.Single", ScriptFieldType::Float },
        { "System.Double", ScriptFieldType::Double },
        { "System.Boolean", ScriptFieldType::Bool },
        { "System.Char", ScriptFieldType::Char },
        { "System.Int16", ScriptFieldType::Short },
        { "System.Int32", ScriptFieldType::Int },
        { "System.Int64", ScriptFieldType::Long },
        { "System.Byte", ScriptFieldType::Byte },
        { "System.UInt16", ScriptFieldType::UShort },
        { "System.UInt32", ScriptFieldType::UInt },
        { "System.UInt64", ScriptFieldType::ULong },

        { "Hazel.Vector2", ScriptFieldType::Vector2 },
        { "Hazel.Vector3", ScriptFieldType::Vector3 },
        { "Hazel.Vector4", ScriptFieldType::Vector4 },

        { "Hazel.Entity", ScriptFieldType::Entity },
    };
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
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);//二进制映像，传入文件数据、文件大小、标志位（这里为1）、状态指针和用户数据

            if (status != MONO_IMAGE_OK)//打开二进制映像时发生错误
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }
            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);//二进制映像，加载程序集
            mono_image_close(image);//关闭图像对象image

            // Don't forget to free the file data
            delete[] fileData;

            return assembly;//返回程序集
        }

        void PrintAssemblyTypes(MonoAssembly* assembly)//传入程序集，打印程序类型
        {
            MonoImage* image = mono_assembly_get_image(assembly);//获取二进制映像
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
        //
        ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
        {
            std::string typeName = mono_type_get_name(monoType);

            auto it = s_ScriptFieldTypeMap.find(typeName);
            if (it == s_ScriptFieldTypeMap.end())//如果在/脚本字段映射没有找到
            {
                HZ_CORE_ERROR("Unknown type: {}", typeName);
                return ScriptFieldType::None;
            }

            return it->second;//找到就返回ScriptFieldType
        }

        const char* ScriptFieldTypeToString(ScriptFieldType type)//字段转化为字符串
        {
            switch (type)
            {
            case ScriptFieldType::Float:   return "Float";
            case ScriptFieldType::Double:  return "Double";
            case ScriptFieldType::Bool:    return "Bool";
            case ScriptFieldType::Char:    return "Char";
            case ScriptFieldType::Byte:    return "Byte";
            case ScriptFieldType::Short:   return "Short";
            case ScriptFieldType::Int:     return "Int";
            case ScriptFieldType::Long:    return "Long";
            case ScriptFieldType::UByte:   return "UByte";
            case ScriptFieldType::UShort:  return "UShort";
            case ScriptFieldType::UInt:    return "UInt";
            case ScriptFieldType::ULong:   return "ULong";
            case ScriptFieldType::Vector2: return "Vector2";
            case ScriptFieldType::Vector3: return "Vector3";
            case ScriptFieldType::Vector4: return "Vector4";
            case ScriptFieldType::Entity:  return "Entity";
            }
            return "<Invalid>";
        }
    }
    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;//根域指针
        MonoDomain* AppDomain = nullptr;//创建应用程序域,c#dll库

        MonoAssembly* CoreAssembly = nullptr;//c#程序域
        MonoImage* CoreAssemblyImage = nullptr;//二进制文件

        MonoAssembly* AppAssembly = nullptr;//app库
        MonoImage* AppAssemblyImage = nullptr;

        ScriptClass EntityClass;//c#程序域中基类，名为entity的类，唯一的一份

        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;//c#基类（包含方法）集合，（LoadAssemblyClasses）
        std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;//实体脚本(调用creat时）
        std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

        //Runtime
        Scene* SceneContext = nullptr;//场景上下文
    };

    static ScriptEngineData* s_Data = nullptr;

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();//创建根域RootDomain
        LoadAssembly("Resources/Scripts/Hazel-ScriptCore.dll");//创建应用程序AppDomain，CoreAssemblyc#程序域，二进制映像CoreAssemblyImage
        LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll");//加载app库
        Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

        LoadAssemblyClasses();//加载assembly.dll中的所有继承Entity的子类，存储在EntityClasses

        //ScriptGlue--Register
        ScriptGlue::RegisterComponents();
        ScriptGlue::RegisterFunctions();
        //
        //获取c#程序域中名为"Hazel"的命名空间和名为"Entity"的类
        s_Data->EntityClass = ScriptClass("Hazel", "Entity", true);//EntityClass = ScriptClass

    }

    void ScriptEngine::InitMono()//调用c#程序域的函数
    {
        mono_set_assemblies_path("mono/lib");

        MonoDomain* rootDomain = mono_jit_init("HazelJITRuntime");//创建一个根域
        HZ_CORE_ASSERT(rootDomain);

        //存储根域指针到s_Data
        s_Data->RootDomain = rootDomain;
    }
    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)//加载应用程序域
    {
        //创建应用程序域
        s_Data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
        mono_domain_set(s_Data->AppDomain, true);//设置为当前域

        //获取c#程序域
        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);//获取image二进制映像
        // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
    }

    void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
    {
        // Move this maybe
        s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
        auto assemb = s_Data->AppAssembly;
        s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
        auto assembi = s_Data->AppAssemblyImage;
        // Utils::PrintAssemblyTypes(s_Data->AppAssembly);
    }

    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete s_Data;
    }
    void ScriptEngine::ShutdownMono()
    {
        // NOTE(Yan): mono is a little confusing to shutdown, so maybe come back to this

        // mono_domain_unload(s_Data->AppDomain);
        s_Data->AppDomain = nullptr;

        // mono_jit_cleanup(s_Data->RootDomain);
        s_Data->RootDomain = nullptr;
    }
    MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)//基类object，"Hazel", "Entity"
    {
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        mono_runtime_object_init(instance);//（并调用构造函数）
        return instance;
    }

    //////////////////////////////////////////////////////////////////
    void ScriptEngine::OnRuntimeStart(Scene* scene)//传入场景（用于ScriptGlue）
    {
        s_Data->SceneContext = scene;
    }

    bool ScriptEngine::EntityClassExists(const std::string& fullClassName)//在EntityClasses找到fullClassName类名键
    {
        return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
    }

    void ScriptEngine::OnCreateEntity(Entity entity)//对具有脚本的实体调用oncreat，
    {
        const auto& sc = entity.GetComponent<ScriptComponent>();//获取当前实体脚本组件
        if (ScriptEngine::EntityClassExists(sc.ClassName))//ClassName为全名，在EntityClasses如果找到了（为entity的派生）
        {
            Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);//脚本类，实体
            s_Data->EntityInstances[entity.GetUUID()] = instance;//EntityInstances存储：实体id-》脚本
            instance->InvokeOnCreate();//调用方法
        }
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)//对实体调用OnUpdate，
    {
        UUID entityUUID = entity.GetUUID();
        HZ_CORE_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());//因为creat时已经注册到EntityInstances

        Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
        instance->InvokeOnUpdate((float)ts);
    }

    Scene* ScriptEngine::GetSceneContext()
    {
        return s_Data->SceneContext;
    }

    Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
    {
        auto it = s_Data->EntityInstances.find(entityID);//
        if (it == s_Data->EntityInstances.end())//不能在EntityInstances找到id
            return nullptr;

        return it->second;//否则返回ScriptInstance
    }

    void ScriptEngine::OnRuntimeStop()
    {
        s_Data->SceneContext = nullptr;

        s_Data->EntityInstances.clear();
    }

    std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
    {
        return s_Data->EntityClasses;
    }

    void ScriptEngine::LoadAssemblyClasses()//加载（assembly.dll）中的所有继承Entity的子类，存储在s_Data->EntityClasses
    {
        s_Data->EntityClasses.clear();//清空

        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);//类型定义表
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);//获取所有类型数量
        MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Hazel", "Entity");//获取到Hazel.Entity类

        for (int32_t i = 0; i < numTypes; i++)//循环所有子类的个数
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);//解码

            const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);//获取命名空间和类
            const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

            std::string fullName;
            if (strlen(nameSpace) != 0)
                fullName = fmt::format("{}.{}", nameSpace, className);//生成完整的类名
            else
                fullName = className;

            MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);//获取对应的Mono类

            if (monoClass == entityClass)//如果当前 == Hazel.Entity类
                continue;//则跳过

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);///是否是实体类的子类（继承的）

            if (!isEntity)//不是子类
                continue;

            Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);//全名（namespace.class)形式，monoclass强制转化为ScriptClass
            
            s_Data->EntityClasses[fullName] = scriptClass;

            // 这个例程是一个迭代器例程，用于检索类中的字段。// 必须传递一个指向 0 的 gpointer，并将其视为一个不透明句柄。
            // 遍历所有元素。当没有更多可用值时，返回值为 NULL。

            int fieldCount = mono_class_num_fields(monoClass);//当前子类字段的数量
            HZ_CORE_WARN("{} has {} fields:", className, fieldCount);
            void* iterator = nullptr;
            while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))//找到字段，当存在时
            {
                const char* fieldName = mono_field_get_name(field);//字段名
                uint32_t flags = mono_field_get_flags(field);
                if (flags & FIELD_ATTRIBUTE_PUBLIC)//如果字段的标志为，公共属性public字段
                {
                    MonoType* type = mono_field_get_type(field);//获取类型
                    ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);//转化为ScriptFieldType类型
                    HZ_CORE_WARN("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));//转化为string类型

                    scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };//存入字段哈希中
                }
            }
        }
        auto& entityClasses = s_Data->EntityClasses;
    }

    MonoImage* ScriptEngine::GetCoreAssemblyImage()
    {
        return s_Data->CoreAssemblyImage;
    }

    ////////////////////////////////////////////////////////////////////////////

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());//从二进制文件找到类
    }

    MonoObject* ScriptClass::Instantiate()//实例化类
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);//从dll中返回m_MonoClass类
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)//获取c#类中的方法
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)//调用方法
    {
        return mono_runtime_invoke(method, instance, params, nullptr);
    }
    ///ScriptInstance/////////////////////////////////////////////////////////////////

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        m_Instance = scriptClass->Instantiate();//从dll中返回m_MonoClass类"Hazel", "Entity"
        //3个方法
        m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);//entity类中的构造
        m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);//OnCreate函数名，参数数量
        m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

        // Call Entity constructor调用实体类构造器
        {
            UUID entityID = entity.GetUUID();
            void* param = &entityID;
            m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);//传入参数
        }
    }

    void ScriptInstance::InvokeOnCreate()//如果OnCreate方法存在，调用
    {
        if (m_OnCreateMethod)
            m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)//如果OnUpdate方法存在，调用
    {
        if (m_OnUpdateMethod)
        {
            void* param = &ts;
            m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
    {
        const auto& fields = m_ScriptClass->GetFields();//从当前脚本，获取哈希字段
        auto it = fields.find(name);//在哈希找到名称
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;//返回
        mono_field_get_value(m_Instance, field.ClassField, buffer);//获取字段值的对象，//要获取值的字段
        return true;
    }

    bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
    {
        const auto& fields = m_ScriptClass->GetFields();
        auto it = fields.find(name);
        if (it == fields.end())
            return false;

        const ScriptField& field = it->second;
        mono_field_set_value(m_Instance, field.ClassField, (void*)value);
        return true;
    }

}