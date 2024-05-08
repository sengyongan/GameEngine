//绘制部分面板
#include "SceneHierarchyPanel.h"
#include"Hazel/Scene/Components.h"
#include"hzpch.h"
#include<imgui/imgui.h>
#include<imgui/imgui_internal.h>
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
        //Hierarchy层级面板////////////////////////////////////////////////////////////////////////
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
        //创建实体菜单
        if (ImGui::BeginPopupContextWindow(0,1,false)) {//0是ID 1是右键
            if (ImGui::MenuItem("create Empty Entity")) {
                m_Context->CreateEntity("Empty Entity");
            }
            ImGui::EndPopup();//当前帧结束块后，下一帧重新创建
        }

        ImGui::End();
        //属性面板//////////////////////////////////////////////////////////////////////////////////
        ImGui::Begin("Properties");
        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);//建立被选中节点的属性面板
            //添加组件按钮
            if (ImGui::Button("Add Component")){
                ImGui::OpenPopup("AddComponent");
            }
            if (ImGui::BeginPopup("AddComponent")) {
                if (ImGui::MenuItem("Camera")) {
                    m_SelectionContext.AddComponent<CameraComponent>();
                        ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Sprite Renderer")) {
                    m_SelectionContext.AddComponent<SpriteRendererComponent>();
                        ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent< TagComponent>().Tag;
        //typedef int 标志，==的这个节点有了ImGuiTreeNodeFlags_Selected标志
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//节点被选中 | 用户点击箭头时，该节点将展开或折叠
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;//可以在非按钮同一行进行选择
        
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity,flags,tag.c_str());//该节点创建四叉树

        if (ImGui::IsItemClicked()) {//当点击哪个节点，表示选中某节点，m_SelectionContex更新为该节点

            m_SelectionContext = entity;
        }
        //删除实体菜单
        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) {//有默认初始值
            if (ImGui::MenuItem("Delete Empty Entity")) {
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }

        if (opened) {//该节点被打开
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;//节点被选中 | 用户点击箭头时，该节点将展开或折叠
            bool opened = ImGui::TreeNodeEx((void*)1, flags, tag.c_str());//创建子节点
            if (opened) {//已经打开
                ImGui::TreePop();//关闭子节点（代码折叠，不绘制节点）

            }
            ImGui::TreePop();//关闭父节点
        }
        if (entityDeleted) {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity)m_SelectionContext = {};//正好选中当前被删除的节点，将选择变为空
        }
    }
    //标签，实际值，重置值，标签宽度
    static void DrawVec3Control(const std::string& lable, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
        //默认字体
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];
        //
        ImGui::PushID(lable.c_str());//用户与 UI 交互时能够正确地处理和更新相应的数据

        ImGui::Columns(2);//第一列标签，第二列3个控件
        ImGui::SetColumnWidth(0,columnWidth);
        ImGui::Text(lable.c_str());
        ImGui::NextColumn();
        
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));//缩进间距

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;//行高度
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
        //////////////
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        //按钮
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        //控件
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");//speed,min,max,格式
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ////////////
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ///////////
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);
        ImGui::PopFont();

        ImGui::SameLine();// 返回上一行，继续横向布局
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    template <typename T, typename UIFunction>
    static void DrawComponent()

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
        //标志
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;//标志 = 默认展开 /  允许项目重叠
        //
        if (entity.HasComponent<TransformComponent>()) {
            //标识符，标志，标签
            //添加ImGuiTreeNodeFlags_DefaultOpen标志（默认被打开），节点名字为Transform
            bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");//尚未创建,会创建并返回 true,

            //
            if (open) {
                auto& tc = entity.GetComponent< TransformComponent>();
                DrawVec3Control("Translation", tc.Translation);//传入初始值
                glm::vec3 rotation = glm::degrees(tc.Rotation);//转为角度制
                DrawVec3Control("Rotation", rotation);
                tc.Rotation = glm::radians(rotation);//转回弧度制
                DrawVec3Control("Scale", tc.Scale, 1.0f);
                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<CameraComponent>()) {
            //标识符，标志，标签
            //创建节点成功ImGuiTreeNodeFlags_DefaultOpen标志，节点名字为Transform
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera")) {
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
                    float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());//保存上一次数据
                    if (ImGui::DragFloat("Vertical FOV", &verticalFov))//每帧重新绘制更新
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


        if (entity.HasComponent<SpriteRendererComponent>()) 
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");
            ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
            //移除组件按钮(应位于渲染imgui之前）
            if (ImGui::Button("+",ImVec2{ 20,20 })) {
                ImGui::OpenPopup("ComponentSettings");
            }
            ImGui::PopStyleVar();

            bool removeComponenet = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Removecomponent")) {
                    removeComponenet = true;
                }
                ImGui::EndPopup();
            }

            //标识符，标志，标签
            //添加ImGuiTreeNodeFlags_DefaultOpen标志（默认被打开），节点名字为Transform
            if (open) {
                auto& src = entity.GetComponent< SpriteRendererComponent>();
                ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));//为节点添加3个浮点控制transform【3]
                ImGui::TreePop();//结束当前节点的绘制。
            }
            //
            if (removeComponenet) {
                entity.RemoveComponent< TransformComponent>();
            }

        }

    }
}