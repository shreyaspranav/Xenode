#pragma once
#include "Core.h"
#include <core/scene/Scene.h>
#include "core/scene/Components.h"
#include "imgui.h"

#include "core/app/Log.h"
#include <glm/gtc/type_ptr.hpp>

class PropertiesPanel {

public:
	PropertiesPanel() {}
	PropertiesPanel(const Xen::Entity& entity) : m_SelectedEntity(entity) {}
	~PropertiesPanel() {}

	inline void SetActiveEntity(const Xen::Entity& entity) { m_SelectedEntity = entity; }
	inline void SetPanelTitle(const std::string& title) { m_PanelTitle = title; }

	inline const std::string& GetPanelTitle() { return m_PanelTitle; }
	inline const Xen::Entity& GetSelectedEntity() { return m_SelectedEntity; }

	void OnImGuiRender()
	{
		//Ortho Camera
		float z_near_point = 0.0f;
		float z_far_point = 0.0f;
		float fov = 0.0f;

		ImGui::Begin(m_PanelTitle.c_str());

		if (!m_SelectedEntity.IsNull())
		{
			Xen::Component::Tag& tag = m_SelectedEntity.GetComponent<Xen::Component::Tag>();
			tag_buf = tag.tag.c_str();

			ImGui::Columns(2, "Tag", false);
			ImGui::SetColumnWidth(0, 92.0f);

			PaddedText(std::string(ICON_FA_TAG) + std::string(" Tag Name: "), 0.0f, 3.0f);
			ImGui::NextColumn();

			ImGui::PushItemWidth(-25.0f);
			if(ImGui::InputText("##Tag Name", (char*)tag_buf, 256))
				tag.tag = std::string(tag_buf);
			ImGui::PopItemWidth();

			ImGui::Columns(1);
			ImGui::SameLine();

			ImGui::PushItemWidth(-0.0f);
			if (ImGui::Button(ICON_FA_CIRCLE_PLUS))
			{
				ImGui::OpenPopup("AddComponent");
			}
			ImGui::PopItemWidth();

			ImGui::Separator();

			if (ImGui::BeginPopup("AddComponent"))
			{
				// Copying a vector every frame ? Inefficient ??
				m_AvailableComponents = m_Components;

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::SpriteRenderer>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), std::string(ICON_FA_TREE) + std::string(" Sprite Renderer"));
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::CameraComp>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), std::string(ICON_FA_CAMERA) + std::string(" Camera"));
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::NativeScript>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), std::string(ICON_FA_CODE) + std::string(" Script"));
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_AvailableComponents.empty())
				{
					ImGui::BeginTooltip();
					ImGui::Text("No More Available Components!!");
					ImGui::EndTooltip();
				}

				for (const std::string& component : m_AvailableComponents)
				{
					if (ImGui::Selectable(component.c_str()))
					{
						if (component.contains("Sprite Renderer"))
							m_SelectedEntity.AddComponent<Xen::Component::SpriteRenderer>();

						else if (component.contains("Camera"))
							m_SelectedEntity.AddComponent<Xen::Component::CameraComp>(std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, 22, 22));

						std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), component);
						m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
					}
					
				}

				ImGui::EndPopup();
			}

			Xen::Component::Transform& transform = m_SelectedEntity.GetComponent<Xen::Component::Transform>();

			ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DefaultOpen;

			if (ImGui::TreeNodeEx((std::string(ICON_FA_CUBES) + std::string("  Transform")).c_str(), tree_flags))
			{
				//PaddedText("Position ", 0.0f, 3.0f); ImGui::SameLine();
				DrawVec3Control("Position", transform.position);
				DrawVec3Control("Rotation", transform.rotation);
				DrawVec3Control("Scale", transform.scale);
				//DrawVec3Control("fdadfas", glm::vec3(1, 1, 1));
				//ImGui::Columns(2, "Pos", false);
				//ImGui::SetColumnWidth(0, 70.0f);
				//PaddedText("Position", 0, 3.0f); ImGui::SameLine();
				//ImGui::NextColumn();
				// 
				//ImGui::PushItemWidth(-0.1f);
				//if (DrawVecControl(1, pos, 0.0f))
				//{
				//	transform.position.x = pos[0];
				//	transform.position.y = pos[1];
				//	transform.position.z = pos[2];
				//}
				//ImGui::PopItemWidth();
				//ImGui::NextColumn();
				//
				//PaddedText("Rotation", 0, 3.0f);; ImGui::SameLine();
				//ImGui::NextColumn();
				//
				//ImGui::PushItemWidth(-0.1f);
				//if (ImGui::DragFloat("##Rotation", &rot, 0.05f));
				//{
				//	transform.rotation = rot;
				//}
				//ImGui::PopItemWidth();
				//ImGui::NextColumn();
				//
				//PaddedText("Scale", 0, 3.0f); ImGui::SameLine();
				//ImGui::NextColumn();
				//
				//ImGui::PushItemWidth(-0.1f);
				//if (ImGui::DragFloat2("##Scale", scale, 0.01f));
				//{
				//	transform.scale.x = scale[0];
				//	transform.scale.y = scale[1];
				//}
				//ImGui::PopItemWidth();
				//
				//ImGui::Columns(1);
				ImGui::TreePop();
			}
			ImGui::Separator();

			backOC:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::CameraComp>())
			{
				if (ImGui::TreeNodeEx((std::string(ICON_FA_CAMERA) + std::string("  Camera")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::CameraComp>();
							ImGui::EndPopup();
							ImGui::TreePop();
							goto backOC;
						}

						ImGui::EndPopup();
					}

					Xen::Component::CameraComp& cam = m_SelectedEntity.GetComponent<Xen::Component::CameraComp>();

					z_near_point = cam.camera->GetNearPoint();
					z_far_point = cam.camera->GetFarPoint();
					fov = cam.camera->GetFovAngle();

					ImGui::Columns(2, "##Camera", false);
					ImGui::SetColumnWidth(0, 110.0f);

					PaddedText("Camera Type ", 0.0f, 3.0f); 
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::Combo("##CameraSelection", &camera_index, "Perspective\0Orthographic\0\0");
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					if (camera_index == 0)
					{
						if (cam.camera->GetProjectionType() != Xen::CameraType::Perspective)
							cam.camera->SetProjectionType(Xen::CameraType::Perspective);

						PaddedText("Fov ", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						if (ImGui::DragFloat("##Fov", &fov, 0.01f))
							cam.camera->SetFovAngle(fov);
						ImGui::PopItemWidth();
						ImGui::NextColumn();

						PaddedText("Z Near Clip ", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						if (ImGui::DragFloat("##ZNearClip", &z_near_point, 0.01f))
							cam.camera->SetNearPoint(z_near_point);
						ImGui::PopItemWidth();
						ImGui::NextColumn();

						PaddedText("Z Far Clip ", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						if (ImGui::DragFloat("##ZFarClip", &z_far_point, 0.01f))
							cam.camera->SetFarPoint(z_far_point);
						ImGui::PopItemWidth();
						ImGui::NextColumn();
					}

					else if (camera_index == 1)
					{
						if (cam.camera->GetProjectionType() != Xen::CameraType::Orthographic)
							cam.camera->SetProjectionType(Xen::CameraType::Orthographic);

						PaddedText("Z Near Clip ", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						if (ImGui::DragFloat("##ZNearClip", &z_near_point, 0.01f))
							cam.camera->SetNearPoint(z_near_point);
						ImGui::PopItemWidth();
						ImGui::NextColumn();

						PaddedText("Z Far Clip ", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						if (ImGui::DragFloat("##ZFarClip", &z_far_point, 0.01f))
							cam.camera->SetFarPoint(z_far_point);
						ImGui::PopItemWidth();
						ImGui::NextColumn();
					}

					PaddedText("Resizable", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::Checkbox("##Resizable", &cam.is_resizable);
					ImGui::NextColumn();
					PaddedText("Primary Camera", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::Checkbox("##Primary Camera", &cam.is_primary_camera);

					ImGui::Columns(1);
					ImGui::TreePop();
				}
				ImGui::Separator();
			}

			backSR:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::SpriteRenderer>())
			{
				if (ImGui::TreeNodeEx((std::string(ICON_FA_TREE) + std::string("  Sprite Renderer")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::SpriteRenderer>();
							ImGui::EndPopup();
							ImGui::TreePop();
							goto backSR;
						}
						ImGui::EndPopup();
					}

					Xen::Component::SpriteRenderer& spriteRenderer = m_SelectedEntity.GetComponent<Xen::Component::SpriteRenderer>();

					float color[4] = { spriteRenderer.color.r, spriteRenderer.color.g, spriteRenderer.color.b, spriteRenderer.color.a };
					char* texture_file_path = (char*)"";
					 
					if(spriteRenderer.texture != nullptr)
						texture_file_path = (char*)spriteRenderer.texture->GetFilePath().c_str();

					ImGui::Columns(2, "SpriteRenderer", false);
					ImGui::SetColumnWidth(0, 80.0f);

					PaddedText("Sprite Color", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					if (ImGui::ColorEdit4("##Sprite color", color))
					{
						spriteRenderer.color.r = color[0];
						spriteRenderer.color.g = color[1];
						spriteRenderer.color.b = color[2];
						spriteRenderer.color.a = color[3];
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();
					PaddedText("Texture", 0.0f, 3.0f);

					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::InputText("##Texture", texture_file_path, IM_ARRAYSIZE(texture_file_path), ImGuiInputTextFlags_ReadOnly);
					ImGui::PopItemWidth();
					
					ImGui::TreePop();

					
				}
				ImGui::Separator();
			}

		}

		ImGui::End();
	}

private:
	
	void PaddedText(const std::string& text, float padding_x, float padding_y)
	{
		ImVec2 sz = ImGui::CalcTextSize(text.c_str());
		ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::InvisibleButton("##padded-text", ImVec2((padding_x * 2) + sz.x, (padding_y * 2) + sz.y));    // ImVec2 operators require imgui_internal.h include and -DIMGUI_DEFINE_MATH_OPERATORS=1
		ImVec2 final_cursor_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(padding_x + cursor.x, padding_y + cursor.y));
		ImGui::Text(text.c_str());
		ImGui::SetCursorPos(final_cursor_pos);
	}

	void DrawVec3Control(const std::string& label, Xen::Vec3& values, float resetValue = 0.0f, float columnWidth = 80.0f)
	{

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, columnWidth);

		PaddedText(label.c_str(), 0.0f, 3.0f);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvailWidth() - 64.5f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}


private:
	const char* tag_buf = "";
	inline static float a = 0.0f;
	inline static float b = 0.0f;

	int camera_index = 1; // 0: Perspective, 1: Orthographic

	float position[3] = {};

	float pad[2] = {};

	std::vector<std::string> m_Components = { std::string(ICON_FA_TREE) + std::string(" Sprite Renderer"),
								std::string(ICON_FA_CAMERA) + std::string(" Camera"), 
								std::string(ICON_FA_CODE) + std::string(" Script") };

	std::vector<std::string> m_AvailableComponents = m_Components;

	Xen::Entity m_SelectedEntity;
	std::string m_PanelTitle = std::string(ICON_FA_INFO) + std::string("  Properties");
};