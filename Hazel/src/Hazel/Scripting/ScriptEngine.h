#pragma once
//���ڽ����ĺ���
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

#include <filesystem>
#include <string>
#include <map>

extern "C" {//����C���Եķ�ʽ���б��������
    typedef struct _MonoClass MonoClass;//struct _MonoClass����Ϊ�µ����� MonoClass
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClassField MonoClassField;
}
namespace Hazel
{    
    enum class ScriptFieldType//�ű��ֶ�����
    {
        None = 0,
        Float, Double,
        Bool, Char, Byte, Short, Int, Long,
        UByte, UShort, UInt, ULong,
        Vector2, Vector3, Vector4,
        Entity
    };

    struct ScriptField//�ű��ֶΣ����ͣ����֣�
    {
        ScriptFieldType Type;
        std::string Name;

        MonoClassField* ClassField;
    };

    // ScriptField + data storage
    struct ScriptFieldInstance//�ű��ֶ�ʵ�����洢���õ��ֶ�ֵ��
    {
        ScriptField Field;

        ScriptFieldInstance()
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));//ȫ����0
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
            static_assert(sizeof(T) <= 16, "Type too large!");//�����8�ֽڣ�����
            memcpy(m_Buffer, &value, sizeof(T));
        }
    private:
        uint8_t m_Buffer[16];

        friend class ScriptEngine;
        friend class ScriptInstance;
    };

    using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;//�ű��ֶ�ӳ��


    //c#�е�"Hazel", "Entity"�� ���࣬�ײ����
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);
        //������c++����c#�ű�,��һЩ����
        MonoObject* Instantiate();//ʵ������
        MonoMethod* GetMethod(const std::string& name, int parameterCount);//��ȡ����
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);//���÷���

        const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }//��ȡ��ϣ�ֶΣ��������ƺ��ֶ��ַ���
    private:
        std::string m_ClassNamespace;//��������ռ���
        std::string m_ClassName;//�������

        std::map<std::string, ScriptField> m_Fields;//��ϣ�ֶ�

        MonoClass* m_MonoClass = nullptr;//��
        
        friend class ScriptEngine;
    };

    //c#�ű�ʵ������������ķ���
    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

        void InvokeOnCreate();//���ؽű�����
        void InvokeOnUpdate(float ts);////���ؽű���������

        Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }//��ȡ�ű�����

        template<typename T>
        T GetFieldValue(const std::string& name)//��ȡ�ű��ֶ�
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            bool success = GetFieldValueInternal(name, s_FieldValueBuffer);//�������ڣ��ű�ʵ����ȡֵ//s_FieldValueBuffer[8]
            if (!success)
                return T();

            return *(T*)s_FieldValueBuffer;//���ؽű��ֶλ���
        }

        template<typename T>
        void SetFieldValue(const std::string& name, const T& value)//�����ֶ�
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            SetFieldValueInternal(name, &value);
        }
    private:
        bool GetFieldValueInternal(const std::string& name, void* buffer);
        bool SetFieldValueInternal(const std::string& name, const	void* value);

    private:
        Ref<ScriptClass> m_ScriptClass;//�ű��е���

        MonoObject* m_Instance = nullptr;///ʵ������
        MonoMethod* m_Constructor = nullptr;//���캯��
        MonoMethod* m_OnCreateMethod = nullptr;//��������
        MonoMethod* m_OnUpdateMethod = nullptr;//���·���

        inline static char s_FieldValueBuffer[16];//�ű��ֶλ���

        friend class ScriptEngine;
        friend struct ScriptFieldInstance;
    };

    //�ű�����ʵĺ���
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const std::filesystem::path& filepath);
        static void LoadAppAssembly(const std::filesystem::path& filepath);

        static void OnRuntimeStart(Scene* scene);//����
        static void OnRuntimeStop();

        static bool EntityClassExists(const std::string& fullClassName);
        static void OnCreateEntity(Entity entity);//������������ô�������
        static void OnUpdateEntity(Entity entity, Timestep ts);

        static Scene* GetSceneContext();
        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);//��ȡʵ��ű�ʵ��

        static Ref<ScriptClass> GetEntityClass(const std::string& name);//��ȡʵ����EntityInstances��Ӧ�� �ű���
        static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();//��ȡEntityInstances��ϣ
        static ScriptFieldMap& GetScriptFieldMap(Entity entity);//��ȡʵ���Ӧ��EntityScriptFields ��ϣֵ

        static MonoImage* GetCoreAssemblyImage();//������ӳ��
    private:
        static void InitMono();
        static void ShutdownMono();

        static MonoObject* InstantiateClass(MonoClass* monoClass);//���ػ���
        static void LoadAssemblyClasses();//���������

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
