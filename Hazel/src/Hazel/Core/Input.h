#include "Hazel/Core/Base.h"
#include"Hazel/Core/KeyCodes.h"
#include"Hazel/Core/MouseButtonCodes.h"
//输入基类
namespace Hazel {
    class  Input {
    public:
        static bool IsKeyPressed(int keycode);//是否按下键盘
        static bool IsMouseButtonPressed(int button);//是否按下鼠标
        static std::pair<float, float>GetMousePosition();//获取鼠标位置
        static float GetMouseX();//获取鼠标x
        static float GetMouseY();
        
    };

}