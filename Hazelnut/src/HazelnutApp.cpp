//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//��

namespace Hazel {
    //client
    class Hazelnut : public Hazel::Application
    {
    public:
        Hazelnut(ApplicationCommandLineArgs args)
            : Application("Hazelnut", args)
        {
            pushlayer(new EditorLayer());//2d��Ⱦ��
        }
        ~Hazelnut()
        {

        }
    };

    //���ؿͻ���
    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        return new Hazelnut(args);
    }
}