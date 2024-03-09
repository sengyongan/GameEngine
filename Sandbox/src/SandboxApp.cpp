#include<Hazel.h>

//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
//
//glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
//{
//    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
//    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
//    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
//    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
//    return Projection * View * Model;
//}

//示例层
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() :Layer("Example")
    {
        //auto cam = camera(5.0f, { 0.5f,0.5f });
    }
    void OnUpdate() override
    {   //如果按下，就输出日志
        if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB)) {//不想依赖GLFW_KEY_TAB
            HZ_CLIENT_TRACE("Tab key is pressed(poll)!");//轮询
        }
    }
    void OnEvent(Hazel::Event& event)override
    {
        if (event.GetEventType() == Hazel::EventType::KeyPressed) {
            Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
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
        pushOverlayer(new Hazel::imGuiLayer());//imGuiLayer层
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