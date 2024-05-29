#include "hzpch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

#include "ScriptGlue.h"

namespace Hazel {
    namespace Utils {
        char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)//�����ļ����ݵ�ָ�룬���ļ���С���ݸ�outSize
        {
            std::ifstream stream(filepath, std::ios::binary | std::ios::ate);//���������Զ�����ģʽ���ļ�ĩβλ�ô��ļ�

            if (!stream)
            {
                // Failed to open the file
                return nullptr;
            }

            std::streampos end = stream.tellg();//��ȡ�ļ�ĩβ��λ�ã�����洢��end������
            stream.seekg(0, std::ios::beg);//���ļ�ָ���ƻ��ļ���ͷ
            uint32_t size = end - stream.tellg();//�����ļ���С����end - ��ǰ�ļ�ָ��λ��

            if (size == 0)//�ļ�Ϊ��
            {
                // File is empty
                return nullptr;
            }

            char* buffer = new char[size];//�ַ�����buffer����СΪ�ļ���С
            stream.read((char*)buffer, size);//���ļ����ݶ�ȡ��buffer��
            stream.close();//�ر��ļ���

            *outSize = size;//��С��ֵ��outSizeָ����ָ��ı���
            return buffer;//���ذ����ļ����ݵ�buffer
        }
        MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)//��·�����س���
        {
            uint32_t fileSize = 0;//���ڴ洢�ļ���С
            char* fileData = ReadBytes(assemblyPath, &fileSize);//��·����ȡ���ַ����飬������С����fileData��

            // NOTE: ���˼��س���֮�⣬�����޷�ʹ�ø�ͼ����Ϊ��ͼ��û�г�������
            MonoImageOpenStatus status;
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);//�����ļ����ݡ��ļ���С����־λ������Ϊ1����״ָ̬����û�����

            if (status != MONO_IMAGE_OK)//��ͼ��ʱ��������
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }
            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);//���س���
            mono_image_close(image);//�ر�ͼ�����image

            // Don't forget to free the file data
            delete[] fileData;

            return assembly;//���س���
        }

        void PrintAssemblyTypes(MonoAssembly* assembly)//������򼯣���ӡ��������
        {
            MonoImage* image = mono_assembly_get_image(assembly);//��ȡͼ�����image
            const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);//��ȡ���ͱ�
            int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);//��ȡ���е�����

            for (int32_t i = 0; i < numTypes; i++)//������������
            {
                uint32_t cols[MONO_TYPEDEF_SIZE];//����
                mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);//��i�����ݽ��뵽cols������

                const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);//���ƿռ�
                const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);//���ƶ�Ӧ���ַ���

                HZ_CORE_TRACE("{}.{}", nameSpace, name);
            }
        }

    }
    struct ScriptEngineData
    {
        MonoDomain* RootDomain = nullptr;//����ָ��
        MonoDomain* AppDomain = nullptr;//����Ӧ�ó�����

        MonoAssembly* CoreAssembly = nullptr;//c#������
        MonoImage* CoreAssemblyImage = nullptr;//�������ͼ��

        ScriptClass EntityClass;//c#����������
    };

    static ScriptEngineData* s_Data = nullptr;

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();
        LoadAssembly("Resources/Scripts/Hazel-ScriptCore.dll");
        
        ScriptGlue::RegisterFunctions();

        //��ȡc#����������Ϊ"Hazel"�������ռ����Ϊ"Main"����
        s_Data->EntityClass = ScriptClass("Hazel", "Entity");

        // 1. ����mono��class����
        MonoObject* instance = s_Data->EntityClass.Instantiate();

        // 2. ���ù���
        MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);//�ҵ�method
        s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);//���ú�����paramsΪ����Ĳ���

        // 3. ���ô������ĺ���
        MonoMethod* printIntFunc = s_Data->EntityClass.GetMethod("PrintInt", 1);

        int value = 5;
        void* param = &value;//����

        s_Data->EntityClass.InvokeMethod(instance, printIntFunc, &param);
        //
        MonoMethod* printIntsFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);//�����������=2
        int value2 = 508;
        void* params[2] =
        {
            &value,
            &value2
        };
        s_Data->EntityClass.InvokeMethod(instance, printIntsFunc, params);
        //
        MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");//Mono�ַ�������
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

    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)//����Ӧ�ó�����
    {
        //����Ӧ�ó�����
        s_Data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
        mono_domain_set(s_Data->AppDomain, true);//����Ϊ��ǰ��

        //��ȡc#������
        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);//��ȡimage
        // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
    }
    void ScriptEngine::InitMono()//����c#������ĺ���
    {
        mono_set_assemblies_path("mono/lib");

        MonoDomain* rootDomain = mono_jit_init("HazelJITRuntime");//����һ������
        HZ_CORE_ASSERT(rootDomain);

        //�洢����ָ�뵽s_Data
        s_Data->RootDomain = rootDomain;
    }

    void ScriptEngine::Shutdown()
    {
        ShutdownMono();
        delete s_Data;
    }
    
    MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)//ʵ������
    {
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        mono_runtime_object_init(instance);//�������ù��캯����
        return instance;
    }

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
    }

    MonoObject* ScriptClass::Instantiate()//ʵ������
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)//��ȡc#���еķ���
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)//���÷���
    {
        return mono_runtime_invoke(method, instance, params, nullptr);
    }
}