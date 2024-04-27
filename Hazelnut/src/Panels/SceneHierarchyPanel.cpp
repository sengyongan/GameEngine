#include "SceneHierarchyPanel.h"
#include"Hazel/Scene/Components.h"
#include"hzpch.h"
#include<imgui/imgui.h>
#include<glm/gtc/type_ptr.hpp>
namespace Hazel {
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
    {
        SetContext(context);
    }
    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
    }
    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        m_Context->m_Registry.each(
            [&](auto entityID) {
                Entity entity{ entityID, m_Context.get() };//m_Context.get()返回场景指针
                DrawEntityNode(entity);
            }
        );
        if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {//点击空白，
            m_SelectionContext = {};//没有选中实体
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);//建立被选中节点的属性面板
        }
        ImGui::End();
    }
    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent< TagComponent>().Tag;
        //typedef int 标志，==的这个节点有了ImGuiTreeNodeFlags_Selected标志
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//节点被选中 | 用户点击箭头时，该节点将展开或折叠
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity,flags,tag.c_str());//该节点创建四叉树

        if (ImGui::IsItemClicked()) {//当点击哪个节点，表示选中某节点，m_SelectionContex更新为该节点

            m_SelectionContext = entity;
        }
        if (opened) {//该节点被打开
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;//节点被选中 | 用户点击箭头时，该节点将展开或折叠
            bool opened = ImGui::TreeNodeEx((void*)1, flags, tag.c_str());//创建子节点
            if (opened) {//已经打开
                ImGui::TreePop();//关闭子节点（代码折叠，不绘制节点）

            }
            ImGui::TreePop();//关闭父节点
        }

    }
    void SceneHierarchyPanel::DrawComponents(Entity entity)//被选中节点
    {
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent< TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));//buffer数组的所有元素都设置为0。
            strcpy_s(buffer, sizeof(buffer), tag.c_str());//将tag字符串复制到buffer数组中。

            if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {//如果输入文本
                tag = std::string(buffer);//将输入的文本给标签
            }
        }
        if (entity.HasComponent<TransformComponent>()) {
            //标识符，标志，标签
            //添加ImGuiTreeNodeFlags_DefaultOpen标志（默认被打开），节点名字为Transform
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
                auto& transform = entity.GetComponent< TransformComponent>().Transform;
                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);//为节点添加3个浮点控制transform【3]
                ImGui::TreePop();//结束当前节点的绘制。
            }

        }
        if (entity.HasComponent<CameraComponent>()) {
            //标识符，标志，标签
            //存在ImGuiTreeNodeFlags_DefaultOpen标志，节点名字为Transform
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera")) {
                //
                auto& cameraComponent = entity.GetComponent< CameraComponent>();//当前被选中的实体获取组件
                auto& camera = cameraComponent.Camera;

                ImGui::Checkbox("Primary", &cameraComponent.Primary);
                //
                const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };//透视 / 正交
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//设置选择默认值：数组索引 0 / 1 
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {//选择

                    for (int i = 0; i < 2; i++) {//选中哪个，currentProjectionTypeString更新为哪个
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];//当前选中的 == 数组中的这一位时，isSelected为真,否则右边不成立为否
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {//创建可选择元素：元素内容，当用户点击该元素时，isSelected更新为true
                            //currentProjectionTypeString = projectionTypeStrings[i];
                            cameraComponent.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                //更改投影矩阵大小（面板）
                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {//透视
                    float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                    if (ImGui::DragFloat("Vertical FOV", &verticalFov))
                        camera.SetPerspectiveVerticalFOV(glm::radians(verticalFov));

                    float orthoNear = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat("Near", &orthoNear))
                        camera.SetPerspectiveNearClip(orthoNear);

                    float orthoFar = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat("Far", &orthoFar))
                        camera.SetPerspectiveFarClip(orthoFar);
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {//正交
                    float orthoSize = camera.GetOrthopraghicSize();
                    if (ImGui::DragFloat("Size", &orthoSize)) {
                        camera.SetOrthopraghicSize(orthoSize);
                    }

                    float orthoNear = camera.GetOrthopraghicNearClip();
                    if (ImGui::DragFloat("Near", &orthoNear)) {
                        camera.SetOrthopraghicNearClip(orthoNear);
                    }

                    float orthoFar = camera.GetOrthopraghicFarClip();
                    if (ImGui::DragFloat("Far", &orthoFar)) {
                        camera.SetOrthopraghicFarClip(orthoFar);
                    }
                    ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
                }

                ImGui::TreePop();//结束当前节点的绘制。
            }
        }
    }
}