//�ͻ���client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//��

namespace Hazel {
    //client
    class HazelEditor : public Hazel::Application
    {
    public:
        HazelEditor() : Application("senlongan Editor")
        {
            pushlayer(new EditorLayer());//2d��Ⱦ��
        }
        ~HazelEditor()
        {

        }
    };

    //���ؿͻ���
    Application* CreatApplication()
    {
        return new HazelEditor();
    }
}