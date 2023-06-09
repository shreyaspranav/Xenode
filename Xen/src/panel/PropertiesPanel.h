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

	inline void SetActiveEntity(const Xen::Entity& entity)								{ m_SelectedEntity = entity; }
	inline void SetPanelTitle(const std::string& title)									{ m_PanelTitle = title; }
	inline void SetTextureLoadDropType(const std::string& texture_load_drop_type)		{ m_TextureLoadDropType = texture_load_drop_type;  }

	inline const std::string& GetPanelTitle() { return m_PanelTitle; }
	inline const Xen::Entity& GetSelectedEntity() { return m_SelectedEntity; }

	void OnImGuiRender()
	{
		//ImGui::ShowDemoWindow();

		//Ortho Camera
		float z_near_point = 0.0f;
		float z_far_point = 0.0f;
		float fov = 0.0f;

		ImGui::Begin(m_PanelTitle.c_str());

		if (!m_SelectedEntity.IsNull() && m_SelectedEntity.IsValid())
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

				if (m_AvailableComponents.empty())
				{
					ImGui::BeginTooltip();
					ImGui::Text("No More Available Components!!");
					ImGui::EndTooltip();
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::SpriteRenderer>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), std::string(ICON_FA_TREE) + std::string(" Sprite Renderer"));
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::CircleRenderer>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), std::string(ICON_FA_CIRCLE) + std::string(" Circle Renderer"));
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

				for (const std::string& component : m_AvailableComponents)
				{
					if (ImGui::Selectable(component.c_str()))
					{

						if (component.contains("Sprite Renderer"))
							m_SelectedEntity.AddComponent<Xen::Component::SpriteRenderer>(Xen::Color(1.0f), nullptr, 1.0f);

						else if (component.contains("Camera"))
							m_SelectedEntity.AddComponent<Xen::Component::CameraComp>(std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, 22, 22));

						else if (component.contains("Circle Renderer"))
							m_SelectedEntity.AddComponent<Xen::Component::CircleRenderer>();

						else if (component.contains("Rigid Body 2D"))
							m_SelectedEntity.AddComponent<Xen::Component::RigidBody2D>();

						else if (component.contains("Box Collider 2D"))
							m_SelectedEntity.AddComponent<Xen::Component::BoxCollider2D>();

						std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), component);
						m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
					}
					
				}

				ImGui::EndPopup();
			}

			Xen::Component::Transform& transform = m_SelectedEntity.GetComponent<Xen::Component::Transform>();

			ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DefaultOpen;

			if (ImGui::CollapsingHeader((std::string(ICON_FA_CUBES) + std::string("  Transform")).c_str(), tree_flags))
			{
				//PaddedText("Position ", 0.0f, 3.0f); ImGui::SameLine();
				DrawVec3Control("Position", transform.position, 0.0f);
				DrawVec3Control("Rotation", transform.rotation, 0.0f);
				DrawVec3Control("Scale", transform.scale, 1.0f);

				//ImGui::TreePop();
			}
			//ImGui::Separator();

			backOC:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::CameraComp>())
			{
				if (ImGui::CollapsingHeader((std::string(ICON_FA_CAMERA) + std::string("  Camera")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::CameraComp>();
							ImGui::EndPopup();
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
						{
							camera_index = 1;
							cam.camera->SetProjectionType(Xen::CameraType::Perspective);
						}

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
						{
							camera_index = 0;
							cam.camera->SetProjectionType(Xen::CameraType::Orthographic);
						}

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

					if (cam.camera->GetProjectionType() == Xen::CameraType::Perspective)
						camera_index = 0;
					else
						camera_index = 1;

					PaddedText("Resizable", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::Checkbox("##Resizable", &cam.is_resizable);
					ImGui::NextColumn();
					PaddedText("Primary Camera", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::Checkbox("##Primary Camera", &cam.is_primary_camera);

					ImGui::Columns(1);
					//ImGui::TreePop();
				}
				//ImGui::Separator();
			}

			backSR:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::SpriteRenderer>())
			{
				if (ImGui::CollapsingHeader((std::string(ICON_FA_TREE) + std::string("  Sprite Renderer")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::SpriteRenderer>();
							ImGui::EndPopup();
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
					ImGui::SetColumnWidth(0, 120.0f);

					PaddedText("Sprite Type", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);

					switch (spriteRenderer.primitive)
					{
					case Xen::SpriteRendererPrimitive::Triangle:
						sprite_renderer_item_index = 0;
						break;
					case Xen::SpriteRendererPrimitive::Quad:
						sprite_renderer_item_index = 1;
						break;
					case Xen::SpriteRendererPrimitive::Polygon:
						sprite_renderer_item_index = 2;
						break;
					default:
						break;
					}

					ImGui::Combo("##Sprite Type", &sprite_renderer_item_index, sprite_renderer_primitives, IM_ARRAYSIZE(sprite_renderer_primitives));
					{
						switch (sprite_renderer_item_index)
						{
						case 0:
							spriteRenderer.primitive = Xen::SpriteRendererPrimitive::Triangle;
							break;
						case 1:
							spriteRenderer.primitive = Xen::SpriteRendererPrimitive::Quad;
							break;
						case 2:
							spriteRenderer.primitive = Xen::SpriteRendererPrimitive::Polygon;
							break;
						default:
							break;
						}
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					if (sprite_renderer_item_index == 2)
					{
						PaddedText("Polygon Segments", 0.0f, 3.0f);
						ImGui::NextColumn();

						int s_count = spriteRenderer.polygon_segment_count;
						if (ImGui::DragInt("##PolygonSegments", &s_count, 0.1f, 5, 50))
							spriteRenderer.polygon_segment_count = s_count;

						ImGui::NextColumn();
					}

					PaddedText("Sprite Color", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs;

					if (ImGui::ColorEdit4("##Sprite color", color, color_edit_flags))
					{
						spriteRenderer.color.r = color[0];
						spriteRenderer.color.g = color[1];
						spriteRenderer.color.b = color[2];
						spriteRenderer.color.a = color[3];
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();
					PaddedText("Texture", 0.0f, 3.0f);

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_TextureLoadDropType.c_str()))
						{
							std::string texture_path = (const char*)payload->Data;
							Xen::Ref<Xen::Texture2D> texture_loaded = Xen::Texture2D::CreateTexture2D(texture_path, true);
							texture_loaded->LoadTexture();

							spriteRenderer.texture = texture_loaded;
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					if (spriteRenderer.texture == nullptr)
						PaddedText("Drag and drop texture here", 0.0, 3.0f);
					else
					{
						float texture_width = ImGui::GetColumnWidth() - 20.0f;

						ImGui::Image((ImTextureID)spriteRenderer.texture->GetNativeTextureID(),
							ImVec2(texture_width, (texture_width * spriteRenderer.texture->GetHeight()) / spriteRenderer.texture->GetHeight()), { 0, 1 }, { 1, 0 });

					}
					ImGui::PopItemWidth();

					if (spriteRenderer.texture != nullptr)
					{
						ImGui::NextColumn();
						PaddedText("Tile Factor", 0.0f, 3.0f);

						ImGui::NextColumn();
						ImGui::PushItemWidth(-0.1f);
						ImGui::DragFloat("##TileFactor", &spriteRenderer.texture_tile_factor, 0.01f, 0.0001f, 100.0f);
						ImGui::PopItemWidth();
					}

					ImGui::Columns(1);
					
					//ImGui::TreePop();
				}
				//ImGui::Separator();
			}

			backCR:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::CircleRenderer>())
			{
				Xen::Component::CircleRenderer& circle_renderer = m_SelectedEntity.GetComponent<Xen::Component::CircleRenderer>();

				if (ImGui::CollapsingHeader((std::string(ICON_FA_CIRCLE) + std::string("  Circle Renderer")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::CircleRenderer>();
							ImGui::EndPopup();
							goto backCR;
						}
						ImGui::EndPopup();
					}
					float circle_color[4] = {
						circle_renderer.color.r,
						circle_renderer.color.g,
						circle_renderer.color.b,
						circle_renderer.color.a,
					};

					ImGui::Columns(2, "##CircleRenderer", false);
					ImGui::SetColumnWidth(0, 100.0f);

					PaddedText("Circle Color", 0.0f, 3.0f);
					ImGui::NextColumn();
					
					ImGui::PushItemWidth(-0.1f);

					ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs;
					if (ImGui::ColorEdit4("##CircleColor", circle_color, color_edit_flags))
					{
						circle_renderer.color.r = circle_color[0];
						circle_renderer.color.g = circle_color[1];
						circle_renderer.color.b = circle_color[2];
						circle_renderer.color.a = circle_color[3];
					}
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Thickness", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##Thickness", &circle_renderer.thickness, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Inner Fade", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##Innerfade", &circle_renderer.inner_fade, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Outer Fade", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##OuterFade", &circle_renderer.outer_fade, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Columns(1);
					//ImGui::TreePop();
				}
				//ImGui::Separator();
			}

			backRB:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::RigidBody2D>())
			{
				if (ImGui::CollapsingHeader((std::string(ICON_FA_CUBES_STACKED) + std::string(" Rigid Body 2D")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::RigidBody2D>();
							ImGui::EndPopup();
							goto backRB;
						}
						ImGui::EndPopup();
					}

					Xen::Component::RigidBody2D& rBody = m_SelectedEntity.GetComponent< Xen::Component::RigidBody2D>();

					ImGui::Columns(2, "##RigidBody2D", false);
					ImGui::SetColumnWidth(0, 120.0f);

					switch (rBody.bodyType)
					{
					case Xen::Component::RigidBody2D::BodyType::Static:
						rigid_body_type_index = 0;
						break;
					case Xen::Component::RigidBody2D::BodyType::Dynamic:
						rigid_body_type_index = 1;
						break;
					case Xen::Component::RigidBody2D::BodyType::Kinematic:
						rigid_body_type_index = 2;
						break;
					default:
						break;
					}

					PaddedText("Body Type", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					if (ImGui::Combo("##BodyType", &rigid_body_type_index, rigid_body_types, IM_ARRAYSIZE(rigid_body_types)))
					{
						switch (rigid_body_type_index)
						{
						case 0:
							rBody.bodyType = Xen::Component::RigidBody2D::BodyType::Static;
							break;
						case 1:
							rBody.bodyType = Xen::Component::RigidBody2D::BodyType::Dynamic;
							break;
						case 2:
							rBody.bodyType = Xen::Component::RigidBody2D::BodyType::Kinematic;
							break;
						default:
							break;
						}
					}
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Fixed Rotation", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::Checkbox("##FixedRotation", &rBody.fixedRotation);

					ImGui::Columns(1);
				}
			}

			backBC:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::BoxCollider2D>())
			{
				if (ImGui::CollapsingHeader((std::string(ICON_FA_SQUARE) + std::string(" Box Collider 2D")).c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponent");

					if (ImGui::BeginPopup("DeleteComponent"))
					{
						if (ImGui::Selectable("Delete Component"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::BoxCollider2D>();
							ImGui::EndPopup();
							goto backBC;
						}
						ImGui::EndPopup();
					}

					Xen::Component::BoxCollider2D& bCollider = m_SelectedEntity.GetComponent<Xen::Component::BoxCollider2D>();

					DrawVec2Control("Size", bCollider.size, 0.5f, 150.0f);
					DrawVec2Control("Offset", bCollider.bodyOffset, 0.0f, 150.0f);

					ImGui::Columns(2, "##BoxCollider2D", false);
					ImGui::SetColumnWidth(0, 150.0f);


					PaddedText("Density", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Density", &bCollider.bodyDensity, 0.05f, 0.0f, 10.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Restitution", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Restitution", &bCollider.bodyRestitution, 0.05f, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Friction", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Friction", &bCollider.bodyFriction, 0.05f, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Restitution Threshold", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##RestitutionThreshold", &bCollider.bodyRestitutionThreshold, 0.05f, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					//DrawVec2Control("Offset", bCollider.bodyOffset);

					ImGui::Columns(1);
				}
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

		ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvailWidth() - 60.0f);
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

	void DrawVec2Control(const std::string& label, Xen::Vec2& values, float resetValue = 0.0f, float columnWidth = 80.0f)
	{

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, columnWidth);

		PaddedText(label.c_str(), 0.0f, 3.0f);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::GetContentRegionAvailWidth() - 40.0f);
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

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}


private:
	const char* tag_buf = "";
	inline static float a = 0.0f;
	inline static float b = 0.0f;

	int sprite_renderer_item_index = 1; // Sprite Renderer Item Index
	const char* sprite_renderer_primitives[3] = { "Triangle", "Quad", "Polygon" };

	int rigid_body_type_index = 0;
	const char* rigid_body_types[3] = { "Static", "Dynamic", "Kinematic" };

	int camera_index = 1; // 0: Perspective, 1: Orthographic
	const char* camera_type[2] = {"Perspective", "Orthographic"};

	float position[3] = {};

	float pad[2] = {};

	std::vector<std::string> m_Components = { 
		std::string(ICON_FA_TREE)				+ std::string(" Sprite Renderer"),
		std::string(ICON_FA_CAMERA)				+ std::string(" Camera"), 
		std::string(ICON_FA_CODE)				+ std::string(" Script"), 
		std::string(ICON_FA_CIRCLE)				+ std::string(" Circle Renderer"), 
		std::string(ICON_FA_CUBES_STACKED)		+ std::string(" Rigid Body 2D"),
		std::string(ICON_FA_SQUARE)				+ std::string(" Box Collider 2D"),
	};

	std::vector<std::string> m_AvailableComponents = m_Components;

	Xen::Entity m_SelectedEntity;
	std::string m_PanelTitle = std::string(ICON_FA_INFO) + std::string("  Properties");

	std::string m_TextureLoadDropType;
};