//客户端client
#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include"EditorLayer.h"//层

namespace Hazel {
    //client
    class HazelEditor : public Hazel::Application
    {
    public:
        HazelEditor() : Application("senlongan Editor")
        {
            pushlayer(new EditorLayer());//2d渲染层
        }
        ~HazelEditor()
        {

        }
    };

    //返回客户端
    Application* CreatApplication()
    {
        return new HazelEditor();
    }
}