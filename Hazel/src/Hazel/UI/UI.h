#pragma once
#include "imgui.h"
//ui������ɫ
namespace Hazel::UI {

    struct ScopedStyleColor//��Χ��ʽ��ɫ
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