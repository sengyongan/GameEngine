//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//��

namespace Hazel {
    //client
    class Hazelnut : public Hazel::Application
    {
    public:
        Hazelnut() : Application("senlongan nut")
        {
            pushlayer(new EditorLayer());//2d��Ⱦ��
        }
        ~Hazelnut()
        {

        }
    };

    //���ؿͻ���
    Application* CreatApplication()
    {
        return new Hazelnut();
    }
}