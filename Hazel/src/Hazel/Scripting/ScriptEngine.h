#pragma once
//���ڽ����ĺ���
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

#include <filesystem>
#include <string>

extern "C" {//����C���Եķ�ʽ���б��������
    typedef struct _MonoClass MonoClass;//struct _MonoClass����Ϊ�µ����� MonoClass
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
}
namespace Hazel
{    
    //c#�е�"Hazel", "Entity"�� ���࣬�ײ����
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className);
        //������c++����c#�ű�,��һЩ����
        MonoObject* Instantiate();//ʵ������
        MonoMethod* GetMethod(const std::string& name, int parameterCount);//��ȡ����
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);//���÷���
    private:
        std::string m_ClassNamespace;//��������ռ���
        std::string m_ClassName;//�������

        MonoClass* m_MonoClass = nullptr;//��
    };

    //c#�ű�ʵ������������ķ���
    class ScriptInstance
    {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

        void InvokeOnCreate();//���ؽű�����
        void InvokeOnUpdate(float ts);////���ؽű���������
    private:
        Ref<ScriptClass> m_ScriptClass;//�ű��е���

        MonoObject* m_Instance = nullptr;///ʵ������
        MonoMethod* m_Constructor = nullptr;//���캯��
        MonoMethod* m_OnCreateMethod = nullptr;//��������
        MonoMethod* m_OnUpdateMethod = nullptr;//���·���
    };

    //�ű�����ʵĺ���
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadAssembly(const std::filesystem::path& filepath);

        static void OnRuntimeStart(Scene* scene);//����
        static void OnRuntimeStop();

        static bool EntityClassExists(const std::string& fullClassName);
        static void OnCreateEntity(Entity entity);//������������ô�������
        static void OnUpdateEntity(Entity entity, Timestep ts);

        static Scene* GetSceneContext();
        static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();

        static MonoImage* GetCoreAssemblyImage();//������ӳ��
    private:
        static void InitMono();
        static void ShutdownMono();

        static MonoObject* InstantiateClass(MonoClass* monoClass);//���ػ���
        static void LoadAssemblyClasses(MonoAssembly* assembly);//���������

        friend class ScriptClass;
        friend class ScriptGlue;
    };

}
