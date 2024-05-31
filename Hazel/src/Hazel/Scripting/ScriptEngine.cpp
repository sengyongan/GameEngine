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
            MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);//������ӳ�񣬴����ļ����ݡ��ļ���С����־λ������Ϊ1����״ָ̬����û�����

            if (status != MONO_IMAGE_OK)//�򿪶�����ӳ��ʱ��������
            {
                const char* errorMessage = mono_image_strerror(status);
                // Log some error message using the errorMessage data
                return nullptr;
            }
            std::string pathString = assemblyPath.string();
            MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);//������ӳ�񣬼��س���
            mono_image_close(image);//�ر�ͼ�����image

            // Don't forget to free the file data
            delete[] fileData;

            return assembly;//���س���
        }

        void PrintAssemblyTypes(MonoAssembly* assembly)//������򼯣���ӡ��������
        {
            MonoImage* image = mono_assembly_get_image(assembly);//��ȡ������ӳ��
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
        MonoDomain* AppDomain = nullptr;//����Ӧ�ó�����,c#dll��

        MonoAssembly* CoreAssembly = nullptr;//c#������
        MonoImage* CoreAssemblyImage = nullptr;//�������ļ�

        ScriptClass EntityClass;//c#�������л��࣬��Ϊentity���࣬Ψһ��һ��

        std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;//c#���ࣨ��������������
        std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;//ʵ��ű�

        //Runtime
        Scene* SceneContext = nullptr;//����������
    };

    static ScriptEngineData* s_Data = nullptr;

    void ScriptEngine::Init()
    {
        s_Data = new ScriptEngineData();

        InitMono();//��������RootDomain
        LoadAssembly("Resources/Scripts/Hazel-ScriptCore.dll");//����Ӧ�ó���AppDomain��CoreAssemblyc#�����򣬶�����ӳ��CoreAssemblyImage
        Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
        LoadAssemblyClasses(s_Data->CoreAssembly);//����assembly.dll�е����м̳�Entity�����࣬�洢��EntityClasses

        //ScriptGlue--Register
        ScriptGlue::RegisterComponents();
        ScriptGlue::RegisterFunctions();
        //
        //��ȡc#����������Ϊ"Hazel"�������ռ����Ϊ"Entity"����
        s_Data->EntityClass = ScriptClass("Hazel", "Entity");//EntityClass = ScriptClass

    }

    void ScriptEngine::InitMono()//����c#������ĺ���
    {
        mono_set_assemblies_path("mono/lib");

        MonoDomain* rootDomain = mono_jit_init("HazelJITRuntime");//����һ������
        HZ_CORE_ASSERT(rootDomain);

        //�洢����ָ�뵽s_Data
        s_Data->RootDomain = rootDomain;
    }
    void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)//����Ӧ�ó�����
    {
        //����Ӧ�ó�����
        s_Data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
        mono_domain_set(s_Data->AppDomain, true);//����Ϊ��ǰ��

        //��ȡc#������
        s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
        s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);//��ȡimage������ӳ��
        // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
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
    MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)//����object��"Hazel", "Entity"
    {
        MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
        mono_runtime_object_init(instance);//�������ù��캯����
        return instance;
    }

    //////////////////////////////////////////////////////////////////
    void ScriptEngine::OnRuntimeStart(Scene* scene)//���볡��������ScriptGlue��
    {
        s_Data->SceneContext = scene;
    }

    bool ScriptEngine::EntityClassExists(const std::string& fullClassName)//��EntityClasses�ҵ�fullClassName������
    {
        return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
    }

    void ScriptEngine::OnCreateEntity(Entity entity)//�Ծ��нű���ʵ�����oncreat��
    {
        const auto& sc = entity.GetComponent<ScriptComponent>();//��ȡ��ǰʵ��ű����
        if (ScriptEngine::EntityClassExists(sc.ClassName))//ClassNameΪȫ������EntityClasses����ҵ��ˣ�Ϊentity��������
        {
            Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);//�ű��࣬ʵ��
            s_Data->EntityInstances[entity.GetUUID()] = instance;//EntityInstances�洢��ʵ��id-���ű�
            instance->InvokeOnCreate();//���÷���
        }
    }

    void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)//��ʵ�����OnUpdate��
    {
        UUID entityUUID = entity.GetUUID();
        HZ_CORE_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());//��Ϊcreatʱ�Ѿ�ע�ᵽEntityInstances

        Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
        instance->InvokeOnUpdate((float)ts);
    }

    Scene* ScriptEngine::GetSceneContext()
    {
        return s_Data->SceneContext;
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

    void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)//���أ�assembly.dll���е����м̳�Entity�����࣬�洢��s_Data->EntityClasses
    {
        s_Data->EntityClasses.clear();//���

        MonoImage* image = mono_assembly_get_image(assembly);//�����������ļ�
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);//���Ͷ����
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);//��ȡ������������
        MonoClass* entityClass = mono_class_from_name(image, "Hazel", "Entity");//��ȡ��Hazel.Entity��

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);//����

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);//��ȡ�����ռ����
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            std::string fullName;
            if (strlen(nameSpace) != 0)
                fullName = fmt::format("{}.{}", nameSpace, name);//��������������
            else
                fullName = name;

            MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);//��ȡ��Ӧ��Mono��

            if (monoClass == entityClass)//�����ǰ == Hazel.Entity��
                continue;//������

            bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);///�Ƿ���ʵ��������ࣨ�̳еģ�
            if (isEntity)//�����࣬�����
                s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);//ȫ����namespace.class)��ʽ��monoclassǿ��ת��ΪScriptClass
        }
    }

    MonoImage* ScriptEngine::GetCoreAssemblyImage()
    {
        return s_Data->CoreAssemblyImage;
    }

    ////////////////////////////////////////////////////////////////////////////

    ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
        : m_ClassNamespace(classNamespace), m_ClassName(className)
    {
        m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());//�Ӷ������ļ��ҵ���
    }

    MonoObject* ScriptClass::Instantiate()//ʵ������
    {
        return ScriptEngine::InstantiateClass(m_MonoClass);//��dll�з���m_MonoClass��
    }

    MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)//��ȡc#���еķ���
    {
        return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
    }

    MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)//���÷���
    {
        return mono_runtime_invoke(method, instance, params, nullptr);
    }
    ///ScriptInstance/////////////////////////////////////////////////////////////////

    ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
        : m_ScriptClass(scriptClass)
    {
        m_Instance = scriptClass->Instantiate();//��dll�з���m_MonoClass��"Hazel", "Entity"
        //3������
        m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);//entity���еĹ���
        m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);//OnCreate����������������
        m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

        // Call Entity constructor����ʵ���๹����
        {
            UUID entityID = entity.GetUUID();
            void* param = &entityID;
            m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);//�������
        }
    }

    void ScriptInstance::InvokeOnCreate()//���OnCreate�������ڣ�����
    {
        if (m_OnCreateMethod)
            m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    }

    void ScriptInstance::InvokeOnUpdate(float ts)//���OnUpdate�������ڣ�����
    {
        if (m_OnUpdateMethod)
        {
            void* param = &ts;
            m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
        }
    }
}