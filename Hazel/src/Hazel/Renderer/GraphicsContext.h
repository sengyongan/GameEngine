#pragma once
//�����Ļ���
namespace Hazel {
    class GraphicsContext {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;//����ǰ�󻺳���
    };
}