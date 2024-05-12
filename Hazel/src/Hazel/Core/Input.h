#include "Hazel/Core/Base.h"
#include"Hazel/Core/KeyCodes.h"
#include"Hazel/Core/MouseButtonCodes.h"
//�������
namespace Hazel {
    class  Input {
    public:
        static bool IsKeyPressed(int keycode);//�Ƿ��¼���
        static bool IsMouseButtonPressed(int button);//�Ƿ������
        static std::pair<float, float>GetMousePosition();//��ȡ���λ��
        static float GetMouseX();//��ȡ���x
        static float GetMouseY();
        
    };

}