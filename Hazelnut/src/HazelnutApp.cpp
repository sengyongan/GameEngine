//客户端client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//层

namespace Hazel {
    //client
    class Hazelnut : public Hazel::Application
    {
    public:
        Hazelnut(const ApplicationSpecification& spec)
            : Application(spec)
        {
            pushlayer(new EditorLayer());//2d渲染层
        }
    };

    //返回客户端
    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.Name = "Hazelnut";
        spec.CommandLineArgs = args;

        return new Hazelnut(spec);
    }
}