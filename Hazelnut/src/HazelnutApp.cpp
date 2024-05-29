//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//��

namespace Hazel {
    //client
    class Hazelnut : public Hazel::Application
    {
    public:
        Hazelnut(const ApplicationSpecification& spec)
            : Application(spec)
        {
            pushlayer(new EditorLayer());//2d��Ⱦ��
        }
    };

    //���ؿͻ���
    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.Name = "Hazelnut";
        spec.CommandLineArgs = args;

        return new Hazelnut(spec);
    }
}