#pragma once
//用于交互的函数
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

#include <filesystem>
#include <string>
#include <map>

extern "C" {//按照C语言的方式进行编译和链接
    typedef struct _MonoClass MonoClass;//struct _MonoClass定义为新的类型 MonoClass
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClassField MonoClassField;
}
namespace Hazel
{    
    enum class ScriptFieldType//脚本字段类型
    {
        None = 0,
        Float, Double,
        Bool, Char, Byte, Short, Int, Long,
        UByte, UShort, UInt, ULong,
        Vector2, Vector3, Vector4,
        Entity
    };

    struct ScriptField//脚本字段（类型，名字）
    {
        ScriptFieldType Type;
        std::string Name;

        MonoClassField* ClassField;
    };

    // ScriptField + data storage
    struct ScriptFieldInstance//脚本字段实例（存储设置的字段值）
    {
        ScriptField Field;

        ScriptFieldInstance()
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));//全部清0
        }

        template<typename T>
        T GetValue()
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            return *(T*)m_Buffer;
        }

        template<typename T>
        void SetValue(T value)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");//如果》8字节，报错
            memcpy(m_Buffer, &value, sizeof(T));
        }
    private:
        uint8_t m_Buffer[16];

        friend class ScriptEngine;
        friend class ScriptInstance;
    };

    using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;//脚本字段映射


    //c#中的"Hazel", "Entity"的 子类，底层操作
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);
        //包含从c++调用c#脚本,的一些操作
        MonoObject* Instantiate();//实例化类
        MonoMethod* GetMethod(const std::string& name, int parameterCount);//获取方法
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);//调用方法

        const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }//获取哈希字段，返回名称和字段字符串
    private:
        std::string m_ClassNamespace;//类的命名空间名
        std::string m_ClassName;//类的类名

        std::map<std::string, ScriptField> m_Fields;//哈希字段

        MonoClass* m_MonoClass = nullptr;//类
        
        friend class ScriptEngine;
    };

    //c#脚本实例，调用子类的方法
    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

        void InvokeOnCreate();//本地脚本创建
        void InvokeOnUpdate(float ts);////本地脚本创建更新

        Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }//获取脚本的类

        template<typename T>
        T GetFieldValue(const std::string& name)//获取脚本字段
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            bool success = GetFieldValueInternal(name, s_FieldValueBuffer);//名字能在，脚本实例获取值//s_FieldValueBuffer[8]
            if (!success)
                return T();

            return *(T*)s_FieldValueBuffer;//返回脚本字段缓冲
        }

        template<typename T>
        void SetFieldValue(const std::string& name, const T& value)//设置字段
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            SetFieldValueInternal(name, &value);
        }
    private:
        bool GetFieldValueInternal(const std::string& name, void* buffer);
        bool SetFieldValueInternal(const std::string& name, const	void* value);

    private:
        Ref<ScriptClass> m_ScriptClass;//脚本中的类

        MonoObject* m_Instance = nullptr;///实例对象
        MonoMethod* m_Constructor = nullptr;//构造函数
        MonoMethod* m_OnCreateMethod = nullptr;//创建方法
        MonoMethod* m_OnUpdateMethod = nullptr;//更新方法

        inline static char s_FieldValueBuffer[16];//脚本字段缓冲

        friend class ScriptEngine;
        friend struct ScriptFieldInstance;
    };

    //脚本间访问的核心
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const std::filesystem::path& filepath);
        static void LoadAppAssembly(const std::filesystem::path& filepath);

        static void OnRuntimeStart(Scene* scene);//场景
        static void OnRuntimeStop();

        static bool EntityClassExists(const std::string& fullClassName);
        static void OnCreateEntity(Entity entity);//调用子类的启用创建方法
        static void OnUpdateEntity(Entity entity, Timestep ts);

        static Scene* GetSceneContext();
        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);//获取实体脚本实例

        static Ref<ScriptClass> GetEntityClass(const std::string& name);//获取实体在EntityInstances对应的 脚本类
        static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();//获取EntityInstances哈希
        static ScriptFieldMap& GetScriptFieldMap(Entity entity);//获取实体对应的EntityScriptFields 哈希值

        static MonoImage* GetCoreAssemblyImage();//二进制映像
    private:
        static void InitMono();
        static void ShutdownMono();

        static MonoObject* InstantiateClass(MonoClass* monoClass);//本地化类
        static void LoadAssemblyClasses();//加载组件类

        friend class ScriptClass;
        friend class ScriptGlue;
    };
    namespace Utils {

        inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
        {
            switch (fieldType)
            {
            case ScriptFieldType::None:    return "None";
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
            HZ_CORE_ASSERT(false, "Unknown ScriptFieldType");
            return "None";
        }

        inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
        {
            if (fieldType == "None")    return ScriptFieldType::None;
            if (fieldType == "Float")   return ScriptFieldType::Float;
            if (fieldType == "Double")  return ScriptFieldType::Double;
            if (fieldType == "Bool")    return ScriptFieldType::Bool;
            if (fieldType == "Char")    return ScriptFieldType::Char;
            if (fieldType == "Byte")    return ScriptFieldType::Byte;
            if (fieldType == "Short")   return ScriptFieldType::Short;
            if (fieldType == "Int")     return ScriptFieldType::Int;
            if (fieldType == "Long")    return ScriptFieldType::Long;
            if (fieldType == "UByte")   return ScriptFieldType::UByte;
            if (fieldType == "UShort")  return ScriptFieldType::UShort;
            if (fieldType == "UInt")    return ScriptFieldType::UInt;
            if (fieldType == "ULong")   return ScriptFieldType::ULong;
            if (fieldType == "Vector2") return ScriptFieldType::Vector2;
            if (fieldType == "Vector3") return ScriptFieldType::Vector3;
            if (fieldType == "Vector4") return ScriptFieldType::Vector4;
            if (fieldType == "Entity")  return ScriptFieldType::Entity;

            HZ_CORE_ASSERT(false, "Unknown ScriptFieldType");
            return ScriptFieldType::None;
        }

    }
}
