#include<Hazel.h>
#include"imgui/imgui.h"
//示例层
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() :Layer("Example")
    {
    }
    void OnUpdate() override {   
        //如果按下，就输出日志
        if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {//不想依赖GLFW_KEY_TAB
            HZ_CLIENT_TRACE("Tab key is pressed(poll)!");//轮询
        }
    }
    virtual void OnImGuiRender()override {//原始非imgui层窗口绘制
        ImGui::Begin("test");
        ImGui::Text("senlongan demo");
        ImGui::End();
    }
    void OnEvent(Hazel::Event& event)override {

        if (event.GetEventType() == Hazel::EventType::KeyPressed) {
            Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;//按压事件
            if (e.GetKeyCode()==HZ_KEY_TAB) {//不想依赖GLFW_KEY_TAB
                HZ_CLIENT_TRACE("Tab key is pressed(event)!");//事件
            }
            HZ_CLIENT_TRACE("{0}", (char)e.GetKeyCode());
        }
    }
};

//client
class Sandbox :public Hazel::Application//继承入口点，相当于创建入口点
{
public:
	Sandbox()
	{
        pushlayer(new ExampleLayer());//创建示例层
        //pushOverlayer(new Hazel::imGuiLayer());//imGuiLayer层
	}
	~Sandbox()
	{

	}
};
//define
//创建应用程序实例
Hazel::Application* Hazel::CreatApplication()
{
	return new Sandbox();
}