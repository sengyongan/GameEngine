#pragma once
//c#�ű�
#include <filesystem>
#include <string>

extern "C" {//����C���Եķ�ʽ���б��������
    typedef struct _MonoClass MonoClass;//struct _MonoClass����Ϊ�µ����� MonoClass
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

        static MonoObject* InstantiateClass(MonoClass* monoClass);//ʵ������

        friend class ScriptClass;
    };
    //��c#�ű���һЩ��������װ
    class ScriptClass
    {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string& classNamespace, const std::string& className);

        MonoObject* Instantiate();//ʵ������
        MonoMethod* GetMethod(const std::string& name, int parameterCount);//��ȡ����
        MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);//���÷���
    private:
        std::string m_ClassNamespace;
        std::string m_ClassName;

        MonoClass* m_MonoClass = nullptr;
    };
}
