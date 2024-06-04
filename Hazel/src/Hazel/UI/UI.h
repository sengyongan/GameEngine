#pragma once
#include "imgui.h"
//ui设置颜色
namespace Hazel::UI {

    struct ScopedStyleColor//范围样式颜色
    {
        ScopedStyleColor() = default;

        ScopedStyleColor(ImGuiCol idx, ImVec4 color, bool predicate = true)
            : m_Set(predicate)
        {
            if (predicate)
                ImGui::PushStyleColor(idx, color);
        }

        ScopedStyleColor(ImGuiCol idx, ImU32 color, bool predicate = true)
            : m_Set(predicate)
        {
            if (predicate)
                ImGui::PushStyleColor(idx, color);
        }

        ~ScopedStyleColor()
        {
            if (m_Set)
                ImGui::PopStyleColor();
        }
    private:
        bool m_Set = false;
    };


}