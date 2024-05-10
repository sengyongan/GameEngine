#include "Hazel/Core/Base.h"
#include"Hazel/Core/KeyCodes.h"
#include"Hazel/Core/MouseButtonCodes.h"
// ‰»Îª˘¿‡
namespace Hazel {
    class  Input {
    public:
        static bool IsKeyPressed(int keycode);
        static bool IsMouseButtonPressed(int button);
        static std::pair<float, float>GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
        
    };

}