#include<Hazel.h>
#include"imgui/imgui.h"
//ʾ����
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() :Layer("Example")
    {
    }
    void OnUpdate() override {   
        //������£��������־
        if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {//��������GLFW_KEY_TAB
            HZ_CLIENT_TRACE("Tab key is pressed(poll)!");//��ѯ
        }
    }
    virtual void OnImGuiRender()override {//ԭʼ��imgui�㴰�ڻ���
        ImGui::Begin("test");
        ImGui::Text("senlongan demo");
        ImGui::End();
    }
    void OnEvent(Hazel::Event& event)override {

        if (event.GetEventType() == Hazel::EventType::KeyPressed) {
            Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;//��ѹ�¼�
            if (e.GetKeyCode()==HZ_KEY_TAB) {//��������GLFW_KEY_TAB
                HZ_CLIENT_TRACE("Tab key is pressed(event)!");//�¼�
            }
            HZ_CLIENT_TRACE("{0}", (char)e.GetKeyCode());
        }
    }
};

//client
class Sandbox :public Hazel::Application//�̳���ڵ㣬�൱�ڴ�����ڵ�
{
public:
	Sandbox()
	{
        pushlayer(new ExampleLayer());//����ʾ����
        //pushOverlayer(new Hazel::imGuiLayer());//imGuiLayer��
	}
	~Sandbox()
	{

	}
};
//define
//����Ӧ�ó���ʵ��
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}