//客户端client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//层

namespace Hazel {
    //client
    class Hazelnut : public Hazel::Application
    {
    public:
        Hazelnut(ApplicationCommandLineArgs args)
            : Application("Hazelnut", args)
        {
            pushlayer(new EditorLayer());//2d渲染层
        }
        ~Hazelnut()
        {

        }
    };

    //返回客户端
    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        return new Hazelnut(args);
    }
}