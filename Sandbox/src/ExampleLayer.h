#pragma once

#include "Hazel.h"

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer();
    virtual ~ExampleLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Hazel::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Hazel::Event& e) override;
private:
    Hazel::ShaderLibrary m_ShaderLibrary;//着色器库
    //三角形
    Hazel::Ref<Hazel::Shader>m_Shader;//着色器类——指针
    Hazel::Ref<Hazel::VertexArray>m_VertexArray;//顶点数组类
    //背景
    Hazel::Ref<Hazel::Shader>m_FlatColorShader;//着色器类
    Hazel::Ref<Hazel::VertexArray>m_SquareVA;//顶点数组类
    //纹理图片
    Hazel::Ref<Hazel::Texture2D>m_Texture, m_awesomefaceTexture;//纹理
    //camera
    Hazel::OrthographicCameraController m_CameraController;
    //背景颜色
    glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};