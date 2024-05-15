#pragma once
//场景层级面板
#include"Hazel/Core/Base.h"
#include"Hazel/Core/Log.h"
#include"Hazel/Scene/Scene.h"
#include"Hazel/Scene/Entity.h"
namespace Hazel {

    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);//设置场景上下文

        void OnImGuiRender();//imgui渲染面板

        Entity GetSelectedEntity() const { return m_SelectionContext; }
        void SetSelectedEntity(Entity entity);
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
    private:
        Ref<Scene> m_Context;//场景
        Entity m_SelectionContext;//当前选择的实体
    };
}

