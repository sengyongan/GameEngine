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
                Entity entity{ entityID, m_Context.get() };//m_Context.get()���س���ָ��
                DrawEntityNode(entity);
            }
        );
        if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {//����հף�
            m_SelectionContext = {};//û��ѡ��ʵ��
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);//������ѡ�нڵ���������
        }
        ImGui::End();
    }
    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent< TagComponent>().Tag;
        //typedef int ��־��==������ڵ�����ImGuiTreeNodeFlags_Selected��־
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//�ڵ㱻ѡ�� | �û������ͷʱ���ýڵ㽫չ�����۵�
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity,flags,tag.c_str());//�ýڵ㴴���Ĳ���

        if (ImGui::IsItemClicked()) {//������ĸ��ڵ㣬��ʾѡ��ĳ�ڵ㣬m_SelectionContex����Ϊ�ýڵ�

            m_SelectionContext = entity;
        }
        if (opened) {//�ýڵ㱻��
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;//�ڵ㱻ѡ�� | �û������ͷʱ���ýڵ㽫չ�����۵�
            bool opened = ImGui::TreeNodeEx((void*)1, flags, tag.c_str());//�����ӽڵ�
            if (opened) {//�Ѿ���
                ImGui::TreePop();//�ر��ӽڵ㣨�����۵��������ƽڵ㣩

            }
            ImGui::TreePop();//�رո��ڵ�
        }

    }
    void SceneHierarchyPanel::DrawComponents(Entity entity)//��ѡ�нڵ�
    {
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent< TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));//buffer���������Ԫ�ض�����Ϊ0��
            strcpy_s(buffer, sizeof(buffer), tag.c_str());//��tag�ַ������Ƶ�buffer�����С�

            if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {//��������ı�
                tag = std::string(buffer);//��������ı�����ǩ
            }
        }
        if (entity.HasComponent<TransformComponent>()) {
            //��ʶ������־����ǩ
            //���ImGuiTreeNodeFlags_DefaultOpen��־��Ĭ�ϱ��򿪣����ڵ�����ΪTransform
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
                auto& transform = entity.GetComponent< TransformComponent>().Transform;
                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);//Ϊ�ڵ����3���������transform��3]
                ImGui::TreePop();//������ǰ�ڵ�Ļ��ơ�
            }

        }
        if (entity.HasComponent<CameraComponent>()) {
            //��ʶ������־����ǩ
            //����ImGuiTreeNodeFlags_DefaultOpen��־���ڵ�����ΪTransform
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera")) {
                //
                auto& cameraComponent = entity.GetComponent< CameraComponent>();//��ǰ��ѡ�е�ʵ���ȡ���
                auto& camera = cameraComponent.Camera;

                ImGui::Checkbox("Primary", &cameraComponent.Primary);
                //
                const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };//͸�� / ����
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];//����ѡ��Ĭ��ֵ���������� 0 / 1 
                if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {//ѡ��

                    for (int i = 0; i < 2; i++) {//ѡ���ĸ���currentProjectionTypeString����Ϊ�ĸ�
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];//��ǰѡ�е� == �����е���һλʱ��isSelectedΪ��,�����ұ߲�����Ϊ��
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {//������ѡ��Ԫ�أ�Ԫ�����ݣ����û������Ԫ��ʱ��isSelected����Ϊtrue
                            //currentProjectionTypeString = projectionTypeStrings[i];
                            cameraComponent.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                //����ͶӰ�����С����壩
                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {//͸��
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

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {//����
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

                ImGui::TreePop();//������ǰ�ڵ�Ļ��ơ�
            }
        }
    }
}