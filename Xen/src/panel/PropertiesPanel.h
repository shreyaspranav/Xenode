#pragma once
#include "Core.h"
#include <core/scene/Scene.h>
#include <core/scene/Components.h>
#include <core/app/Log.h>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <ImGradientHDR.h>

#include "StringValues.h"

class PropertiesPanel {

public:
	PropertiesPanel() {}
	PropertiesPanel(const Xen::Entity& entity) : m_SelectedEntity(entity) {}
	~PropertiesPanel() {}

	inline void SetActiveEntity(const Xen::Entity& entity)								{ m_SelectedEntity = entity; }
	inline void SetPanelTitle(const std::string& title)									{ m_PanelTitle = title; }
	inline void SetTextureLoadDropType(const std::string& texture_load_drop_type)		{ m_TextureLoadDropType = texture_load_drop_type; }
	inline void SetScriptLoadDropType(const std::string& script_load_drop_type)			{ m_ScriptLoadDropType = script_load_drop_type;  }

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
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_SPRITE_RENDERER);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::PointLight>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_POINT_LIGHT);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::AmbientLight>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_AMBIENT_LIGHT);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::CameraComp>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_CAMERA);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::NativeScript>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_NATIVE_SCRIPT);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::ScriptComp>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_SCRIPT);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::BoxCollider2D>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_BOX_COLLIDER_2D);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::CircleCollider2D>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_CIRCLE_COLLIDER_2D);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::RigidBody2D>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_RIGID_BODY_2D);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				if (m_SelectedEntity.HasAnyComponent<Xen::Component::ParticleSystem2DComp>())
				{
					std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), Xen::StringValues::COMPONENT_PARTICLE_SYSTEM_2D);
					m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
				}

				for (int i = 0; i < m_AvailableComponents.size(); i++)
				{
					if (ImGui::Selectable(m_AvailableComponents[i].c_str()))
					{

						if (m_AvailableComponents[i].contains("Sprite Renderer"))
							m_SelectedEntity.AddComponent<Xen::Component::SpriteRenderer>(Xen::Color(1.0f), nullptr, 1.0f);

						else if (m_AvailableComponents[i].contains("Camera"))
							m_SelectedEntity.AddComponent<Xen::Component::CameraComp>(std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, 22, 22));

						else if (m_AvailableComponents[i].contains("Point Light"))
							m_SelectedEntity.AddComponent<Xen::Component::PointLight>();

						else if (m_AvailableComponents[i].contains("Ambient Light"))
							m_SelectedEntity.AddComponent<Xen::Component::AmbientLight>();

						else if (m_AvailableComponents[i].contains("Native Script"))
							m_SelectedEntity.AddComponent<Xen::Component::NativeScript>();

						else if (m_AvailableComponents[i].contains(" Script"))
							m_SelectedEntity.AddComponent<Xen::Component::ScriptComp>();

						else if (m_AvailableComponents[i].contains("Rigid Body 2D"))
							m_SelectedEntity.AddComponent<Xen::Component::RigidBody2D>();

						else if (m_AvailableComponents[i].contains("Box Collider 2D"))
							m_SelectedEntity.AddComponent<Xen::Component::BoxCollider2D>();

						else if (m_AvailableComponents[i].contains("Circle Collider 2D"))
							m_SelectedEntity.AddComponent<Xen::Component::CircleCollider2D>();

						else if (m_AvailableComponents[i].contains("Particle System 2D"))
							m_SelectedEntity.AddComponent<Xen::Component::ParticleSystem2DComp>();

						std::remove(m_AvailableComponents.begin(), m_AvailableComponents.end(), m_AvailableComponents[i]);
						m_AvailableComponents.resize(m_AvailableComponents.size() - 1);
					}
				}

				ImGui::EndPopup();
			}

			Xen::Component::Transform& transform = m_SelectedEntity.GetComponent<Xen::Component::Transform>();

			ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DefaultOpen;

			if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_TRANSFORM.c_str(), tree_flags))
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
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_CAMERA.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentCamera");

					if (ImGui::BeginPopup("DeleteComponentCamera"))
					{
						if (ImGui::Selectable("Delete Component: Camera"))
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
				typedef Xen::Component::SpriteRenderer::Primitive SpritePrimitive;
				typedef Xen::Component::SpriteRenderer::CircleProperties CircleProperties;
				typedef Xen::Component::SpriteRenderer::PolygonProperties PolygonProperties;

				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_SPRITE_RENDERER.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentSpriteRenderer");

					if (ImGui::BeginPopup("DeleteComponentSpriteRenderer"))
					{
						if (ImGui::Selectable("Delete Component: Sprite Renderer"))
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
					case SpritePrimitive::Triangle:
						sprite_renderer_item_index = 0;
						break;
					case SpritePrimitive::Quad:
						sprite_renderer_item_index = 1;
						break;
					case SpritePrimitive::Polygon:
						sprite_renderer_item_index = 2;
						break;
					case SpritePrimitive::Circle:
						sprite_renderer_item_index = 3;
						break;
					default:
						break;
					}

					ImGui::Combo("##Sprite Type", &sprite_renderer_item_index, sprite_renderer_primitives, IM_ARRAYSIZE(sprite_renderer_primitives));
					{
						switch (sprite_renderer_item_index)
						{
						case 0:
							spriteRenderer.primitive = SpritePrimitive::Triangle;
							break;
						case 1:
							spriteRenderer.primitive = SpritePrimitive::Quad;
							break;
						case 2:
							spriteRenderer.primitive = SpritePrimitive::Polygon;
							break;
						case 3:
							spriteRenderer.primitive = SpritePrimitive::Circle;
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

						int s_count = spriteRenderer.polygon_properties.segment_count;
						if (ImGui::DragInt("##PolygonSegments", &s_count, 0.1f, 5, 50))
							spriteRenderer.polygon_properties.segment_count = s_count;

						ImGui::NextColumn();
					}

					else if (sprite_renderer_item_index == 3)
					{
						PaddedText("Thickness", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						ImGui::SliderFloat("##Thickness", &spriteRenderer.circle_properties.thickness, 0.0f, 1.0f);
						ImGui::PopItemWidth();
						ImGui::NextColumn();

						PaddedText("Inner Fade", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						ImGui::SliderFloat("##Innerfade", &spriteRenderer.circle_properties.innerfade, 0.0f, 1.0f);
						ImGui::PopItemWidth();
						ImGui::NextColumn();

						PaddedText("Outer Fade", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::PushItemWidth(-0.1f);
						ImGui::SliderFloat("##OuterFade", &spriteRenderer.circle_properties.outerfade, 0.0f, 1.0f);
						ImGui::PopItemWidth();
						ImGui::NextColumn();
					}

					PaddedText("Sprite Color", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

					if (ImGui::ColorEdit4("##Sprite color", color, color_edit_flags))
					{
						spriteRenderer.color.r = color[0];
						spriteRenderer.color.g = color[1];
						spriteRenderer.color.b = color[2];
						spriteRenderer.color.a = color[3];
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();
					
					PaddedText("Alpha", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##Alpha", &spriteRenderer.color.a, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					// TEMP: Add support for textured circles:

					if (sprite_renderer_item_index != 3)
					{
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
					}

					ImGui::Columns(1);
					
					//ImGui::TreePop();
				}
				//ImGui::Separator();
			}

			backPL:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::PointLight>())
			{
				Xen::Component::PointLight& point_light = m_SelectedEntity.GetComponent<Xen::Component::PointLight>();

				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_POINT_LIGHT.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentPointLight");

					if (ImGui::BeginPopup("DeleteComponentPointLight"))
					{
						if (ImGui::Selectable("Delete Component: Point Light"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::PointLight>();
							ImGui::EndPopup();
							goto backPL;
						}
						ImGui::EndPopup();
					}
					float light_color[4] = {
						point_light.lightColor.r,
						point_light.lightColor.g,
						point_light.lightColor.b,
						point_light.lightColor.a,
					};

					ImGui::Columns(2, "##PointLight", false);
					ImGui::SetColumnWidth(0, 100.0f);

					PaddedText("Light Color", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);

					ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha;
					if (ImGui::ColorEdit4("##LightColor", light_color, color_edit_flags))
					{
						point_light.lightColor.r = light_color[0];
						point_light.lightColor.g = light_color[1];
						point_light.lightColor.b = light_color[2];
						point_light.lightColor.a = light_color[3];
					}
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Cast Shadows", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::Checkbox("##CastShadows", &point_light.castShadow);
					ImGui::NextColumn();

					PaddedText("Radius", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Radius", &point_light.radius, 0.01f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Intensity", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Intensity", &point_light.intensity, 0.01f, 200.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Falloff A", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##FallofA", &point_light.fallofA, 0.1f, 10.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Falloff B", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##FalloffB", &point_light.fallofB, 0.1f, 10.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Columns(1);
					//ImGui::TreePop();
				}
				//ImGui::Separator();
			}

			backAL:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::AmbientLight>())
			{
				Xen::Component::AmbientLight& ambient_light = m_SelectedEntity.GetComponent<Xen::Component::AmbientLight>();

				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_AMBIENT_LIGHT.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentAmbientLight");

					if (ImGui::BeginPopup("DeleteComponentAmbientLight"))
					{
						if (ImGui::Selectable("Delete Component: Ambient Light"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::AmbientLight>();
							ImGui::EndPopup();
							goto backAL;
						}
						ImGui::EndPopup();
					}
					float light_color[4] = {
						ambient_light.color.r,
						ambient_light.color.g,
						ambient_light.color.b,
						ambient_light.color.a,
					};

					ImGui::Columns(2, "##AmbientLight", false);
					ImGui::SetColumnWidth(0, 100.0f);

					PaddedText("Color", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);

					ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha;
					if (ImGui::ColorEdit4("##LightColor", light_color, color_edit_flags))
					{
						ambient_light.color.r = light_color[0];
						ambient_light.color.g = light_color[1];
						ambient_light.color.b = light_color[2];
						ambient_light.color.a = light_color[3];
					}
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Intensity", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::SliderFloat("##Intensity", &ambient_light.intensity, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Columns(1);
				}
				//ImGui::Separator();
			}

			backRB:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::RigidBody2D>())
			{
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_RIGID_BODY_2D.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentRigidBody2D");

					if (ImGui::BeginPopup("DeleteComponentRigidBody2D"))
					{
						if (ImGui::Selectable("Delete Component: Rigid Body 2D"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::RigidBody2D>();
							ImGui::EndPopup();
							goto backRB;
						}
						ImGui::EndPopup();
					}

					Xen::Component::RigidBody2D& rBody = m_SelectedEntity.GetComponent<Xen::Component::RigidBody2D>();

					ImGui::Columns(2, "##RigidBody2D", false);
					ImGui::SetColumnWidth(0, 120.0f);

					switch (rBody.bodyType)
					{
					case Xen::BodyType2D::Static:
						rigid_body_type_index = 0;
						break;
					case Xen::BodyType2D::Dynamic:
						rigid_body_type_index = 1;
						break;
					case Xen::BodyType2D::Kinematic:
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
							rBody.bodyType = Xen::BodyType2D::Static;
							break;
						case 1:
							rBody.bodyType = Xen::BodyType2D::Dynamic;
							break;
						case 2:
							rBody.bodyType = Xen::BodyType2D::Kinematic;
							break;
						default:
							break;
						}
					}
					ImGui::PopItemWidth();
					ImGui::NextColumn();


					PaddedText("Mass", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Mass", &rBody.physicsMaterial.mass, 0.05f, 0.001f, 0.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Friction", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Friction", &rBody.physicsMaterial.friction, 0.05f, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Restitution", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Restitution", &rBody.physicsMaterial.restitution, 0.05f, 0.0f, 1.0f);
					ImGui::PopItemWidth();
					ImGui::NextColumn();

					PaddedText("Restitution Threshold", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##RestitutionThreshold", &rBody.physicsMaterial.restitutionThreshold, 0.05f, 0.0f, 10000.0f);
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					PaddedText("Fixed Rotation", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::Checkbox("##FixedRotation", &rBody.physicsMaterial.fixedRotation);

					ImGui::Columns(1);
				}
			}

			backBC:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::BoxCollider2D>())
			{
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_BOX_COLLIDER_2D.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentBoxCollider2D");

					if (ImGui::BeginPopup("DeleteComponentBoxCollider2D"))
					{
						if (ImGui::Selectable("Delete Component: Box Collider 2D"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::BoxCollider2D>();
							ImGui::EndPopup();
							goto backBC;
						}
						ImGui::EndPopup();
					}

					Xen::Component::BoxCollider2D& bCollider = m_SelectedEntity.GetComponent<Xen::Component::BoxCollider2D>();

					DrawVec2Control("Size Scale", bCollider.sizeScale, 0.5f, 120.0f);
					DrawVec2Control("Offset", bCollider.bodyOffset, 0.0f, 120.0f);
				}
			}

			backCC:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::CircleCollider2D>())
			{
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_CIRCLE_COLLIDER_2D.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentCircleCollider2D");

					if (ImGui::BeginPopup("DeleteComponentCircleCollider2D"))
					{
						if (ImGui::Selectable("Delete Component: Box Collider 2D"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::BoxCollider2D>();
							ImGui::EndPopup();
							goto backCC;
						}
						ImGui::EndPopup();
					}

					Xen::Component::CircleCollider2D& cCollider = m_SelectedEntity.GetComponent<Xen::Component::CircleCollider2D>();

					ImGui::Columns(2, "##RigidBody2D", false);
					ImGui::SetColumnWidth(0, 120.0f);

					PaddedText("Radius Scale", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##Radius", &cCollider.radiusScale, 0.05f, 0.001f, 150.0f);
					ImGui::PopItemWidth();

					ImGui::Columns(1);

					DrawVec2Control("Offset", cCollider.bodyOffset, 0.0f, 120.0f);
				}
			}

		backNS:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::NativeScript>())
			{
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_NATIVE_SCRIPT.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentNativeScript");

					if (ImGui::BeginPopup("DeleteComponentNativeScript"))
					{
						if (ImGui::Selectable("Delete Component: Native Script"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::NativeScript>();
							ImGui::EndPopup();
							goto backNS;
						}
						ImGui::EndPopup();
					}

					PaddedText("To Be Implemented!", 0.0f, 3.0f);
				}
			}

			backS:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::ScriptComp>())
			{
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_SCRIPT.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentScript");

					if (ImGui::BeginPopup("DeleteComponentScript"))
					{
						if (ImGui::Selectable("Delete Component: Script"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::ScriptComp>();
							ImGui::EndPopup();
							goto backS;
						}
						ImGui::EndPopup();
					}

					Xen::Component::ScriptComp& script_comp = m_SelectedEntity.GetComponent<Xen::Component::ScriptComp>();

					ImGui::Columns(2, "##Script", false);

					PaddedText("Script", 0.0f, 3.0f);
					ImGui::NextColumn();
					ImGui::SetColumnWidth(0, 120.0f);

					ImGui::PushItemWidth(-0.1f);
					ImGui::InputText("##Script", (char*)script_comp.script_file_path.c_str(), script_comp.script_file_path.size() + 1, ImGuiInputTextFlags_ReadOnly);

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_ScriptLoadDropType.c_str()))
						{
							std::string script_path = (const char*)payload->Data;

							script_comp.script_instance = Xen::Script::CreateScript(script_path);
							script_comp.script_file_path = script_path;
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::PopItemWidth();
					ImGui::NextColumn();
					ImGui::Columns(1);

					ImGui::Separator();
					ImGui::Columns(2, "##Script", false);
					PaddedText("Public Fields:", 0.0f, 3.0f);
					ImGui::Columns(1);
				}
			}

			backPS:
			if (m_SelectedEntity.HasAnyComponent<Xen::Component::ParticleSystem2DComp>())
			{
				if (ImGui::CollapsingHeader(Xen::StringValues::COMPONENT_PARTICLE_SYSTEM_2D.c_str(), tree_flags))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						ImGui::OpenPopup("DeleteComponentParticleSystem2D");

					if (ImGui::BeginPopup("DeleteComponentParticleSystem2D"))
					{
						if (ImGui::Selectable("Delete Component: Particle System 2D"))
						{
							m_SelectedEntity.DeleteComponent<Xen::Component::ParticleSystem2DComp>();
							ImGui::EndPopup();
							goto backPS;
						}
						ImGui::EndPopup();
					}

					Xen::Component::ParticleSystem2DComp& particleSystem = m_SelectedEntity.GetComponent<Xen::Component::ParticleSystem2DComp>();
					Xen::ParticleSettings2D& particleSettings2D = particleSystem.particleInstance.particleSettings;

					// TODO: Fix the transform situation:
					Xen::Component::Transform& transform = m_SelectedEntity.GetComponent<Xen::Component::Transform>();
					particleSettings2D.position = transform.position;

					ImGui::Columns(2, "##PC", false);
					ImGui::SetColumnWidth(0, 120.0f);

					PaddedText("Particle Count", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					int rate = particleSettings2D.rate;
					if (ImGui::DragInt("##ParticleCount", &rate, 1.0f))
						particleSettings2D.rate = rate;
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					PaddedText("Size Range", 0.0f, 3.0f);
					ImGui::NextColumn();

					float sizeRange[] = {
						particleSettings2D.sizeRandomRange.x,
						particleSettings2D.sizeRandomRange.y
					};

					ImGui::PushItemWidth(-0.1f);
					if (ImGui::DragFloat2("##SizeRange", sizeRange, 0.01f, 0.001f, 10.0f))
						particleSettings2D.sizeRandomRange = {sizeRange[0], sizeRange[1]};
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					PaddedText("Lifetime Range", 0.0f, 3.0f);
					ImGui::NextColumn();

					float lifeRange[] = {
						particleSettings2D.lifeRandomRange.x,
						particleSettings2D.lifeRandomRange.y
					};

					ImGui::PushItemWidth(-0.1f);
					if (ImGui::DragFloat2("##LifeRange", lifeRange, 0.01f, 1.0f, 50.0f))
						particleSettings2D.lifeRandomRange = { lifeRange[0], lifeRange[1] };
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					PaddedText("Velocity Range", 0.0f, 3.0f);
					ImGui::NextColumn();

					float velocityRange[] = {
						particleSettings2D.velocityRandomRange.x,
						particleSettings2D.velocityRandomRange.y
					};

					ImGui::PushItemWidth(-0.1f);
					if (ImGui::DragFloat2("##VelocityRange", velocityRange, 0.01f, 0.001f, 10.0f))
						particleSettings2D.velocityRandomRange = { velocityRange[0], velocityRange[1] };
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					PaddedText("Velocity Damping", 0.0f, 3.0f);
					ImGui::NextColumn();

					ImGui::PushItemWidth(-0.1f);
					ImGui::DragFloat("##VelocityDamping", &particleSettings2D.velocityDamping, 0.01f, 0.0f, 10.0f);
					ImGui::PopItemWidth();

					ImGui::Columns(1);

					ImGuiTreeNodeFlags particleSettingsTreeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth;

					// if (ImGui::TreeNodeEx("Color", particleSettingsTreeFlags))
					{
						state = ImGradientHDRState();
						Xen::ColorGradient& tempGradient = particleSettings2D.colorGradient;

						for (auto&& it = tempGradient.ColorBegin(); it != tempGradient.ColorEnd(); ++it)
						{
							const Xen::Color& color = (*it).color;
							state.AddColorMarker((*it).sliderValue, { color.r, color.g, color.b }, 1.0f);
						}

						for (auto&& it = tempGradient.AlphaBegin(); it != tempGradient.AlphaEnd(); ++it)
						{
							state.AddAlphaMarker((*it).sliderValue, (*it).alpha);
						}

						ImGradientHDR(10, state, tempState, true);

						if (tempState.draggingMarkerType == ImGradientHDRMarkerType::Color)
						{
							auto&& marker = state.GetColorMarker(tempState.draggingIndex);
							tempGradient.EditColorPosition({ marker->Color[0], marker->Color[1], marker->Color[2], 1.0f }, marker->Position);
						}

						else if (tempState.draggingMarkerType == ImGradientHDRMarkerType::Alpha)
						{
							auto&& marker = state.GetAlphaMarker(tempState.draggingIndex);
							tempGradient.EditAlphaPosition(marker->Alpha, marker->Position);
						}


						if (tempState.selectedMarkerType == ImGradientHDRMarkerType::Color)
						{
							// Get the index and the color of that selected color slider:

							auto selectedColorMarker = state.GetColorMarker(tempState.selectedIndex);
							if (selectedColorMarker)
							{
								if (ImGui::ColorPicker3("##Color", selectedColorMarker->Color.data()))
									tempGradient.EditColor(selectedColorMarker->Position, { selectedColorMarker->Color[0], selectedColorMarker->Color[1], selectedColorMarker->Color[2], 1.0f });
							}
						}

						ImGui::Columns(2, "##PSAlpha", false);
						ImGui::SetColumnWidth(0, 120.0f);

						if (tempState.selectedMarkerType == ImGradientHDRMarkerType::Alpha)
						{
							PaddedText("Alpha", 0.0f, 3.0f);
							ImGui::NextColumn();

							// Get the index and the color of that selected color slider:

							auto selectedAlphaMarker = state.GetAlphaMarker(tempState.selectedIndex);
							if (selectedAlphaMarker)
							{
								ImGui::PushItemWidth(-0.1f);
								if (ImGui::SliderFloat("##Alpha", &selectedAlphaMarker->Alpha, 0.0f, 1.0f))
									tempGradient.EditAlpha(selectedAlphaMarker->Position, selectedAlphaMarker->Alpha);
								ImGui::PopItemWidth();
							}
							ImGui::NextColumn();
							ImGui::Columns(1);
						}

						if (state.ColorCount > tempGradient.GetColorsSize())
						{
							for (int i = 0; i < state.ColorCount; i++)
							{
								auto color = state.GetColorMarker(i)->Color;
								float sliderValue = state.GetColorMarker(i)->Position;

								Xen::ColorGradient::ColorSlider slider = { { color[0], color[1], color[2], 1.0f }, sliderValue };
								if (std::find(tempGradient.ColorBegin(), tempGradient.ColorEnd(), slider) == tempGradient.ColorEnd())
									tempGradient.AddColor(slider.color, slider.sliderValue);
							}
						}

						if (state.AlphaCount > tempGradient.GetAlphasSize())
						{
							for (int i = 0; i < state.AlphaCount; i++)
							{
								float alpha = state.GetAlphaMarker(i)->Alpha;
								float sliderValue = state.GetAlphaMarker(i)->Position;

								Xen::ColorGradient::AlphaSlider slider = { alpha, sliderValue };
								if (std::find(tempGradient.AlphaBegin(), tempGradient.AlphaEnd(), slider) == tempGradient.AlphaEnd())
									tempGradient.AddAlpha(slider.alpha, slider.sliderValue);
							}
						}

						int affectColorByRadioIndex;

						switch (particleSettings2D.affectColorBy)
						{
							case Xen::AffectColorBy::Distance: affectColorByRadioIndex = 0; break;
							case Xen::AffectColorBy::Lifetime: affectColorByRadioIndex = 1; break;
							case Xen::AffectColorBy::Random:   affectColorByRadioIndex = 2; break;
						}

						ImGui::Columns(2, "##RadioButtonThingy", false);
						ImGui::SetColumnWidth(0, 120.0f);

						PaddedText("Affect By", 0.0f, 3.0f);
						ImGui::NextColumn();

						ImGui::RadioButton("Distance", &affectColorByRadioIndex, 0);
								
						ImGui::NextColumn();
						ImGui::NextColumn();

						ImGui::RadioButton("Lifetime", &affectColorByRadioIndex, 1);

						ImGui::NextColumn();
						ImGui::NextColumn();

						ImGui::RadioButton("Random", &affectColorByRadioIndex, 2);

						switch (affectColorByRadioIndex)
						{
							case 0: particleSettings2D.affectColorBy = Xen::AffectColorBy::Distance; break;
							case 1: particleSettings2D.affectColorBy = Xen::AffectColorBy::Lifetime; break;
							case 2: particleSettings2D.affectColorBy = Xen::AffectColorBy::Random;   break;
						}

						ImGui::NextColumn();

						if (affectColorByRadioIndex == 0)
						{
							PaddedText("Distance Scale", 0.0f, 3.0f);
							ImGui::NextColumn();

							ImGui::PushItemWidth(-0.1f);
							ImGui::DragFloat("##DistanceFactor", &particleSettings2D.colorDistanceScale, 0.1f, 1.0f, 0.0f);
							ImGui::PopItemWidth();
						}
						
						ImGui::Columns(1);

						// ImGui::TreePop();
					}

					ImGui::Separator();
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

		ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x - 60.0f);
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

		ImGui::PushMultiItemsWidths(2, ImGui::GetContentRegionAvail().x - 40.0f);
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

	int sprite_renderer_item_index = 1; // Sprite Renderer Item Index
	const char* sprite_renderer_primitives[4] = { 
		Xen::StringValues::DROPDOWN_SPRITETYPE_TRIANGLE.c_str(), 
		Xen::StringValues::DROPDOWN_SPRITETYPE_QUAD.c_str(),
		Xen::StringValues::DROPDOWN_SPRITETYPE_POLYGON.c_str(),
		Xen::StringValues::DROPDOWN_SPRITETYPE_CIRCLE.c_str() 
	};

	int rigid_body_type_index = 0;
	const char* rigid_body_types[3] = { "Static", "Dynamic", "Kinematic" };

	int camera_index = 1; // 0: Perspective, 1: Orthographic
	const char* camera_type[2] = { "Perspective", "Orthographic" };

	float position[3] = {};

	float pad[2] = {};

	std::vector<std::string> m_Components = { 
		Xen::StringValues::COMPONENT_SPRITE_RENDERER,
		Xen::StringValues::COMPONENT_CAMERA,
		Xen::StringValues::COMPONENT_NATIVE_SCRIPT,
		Xen::StringValues::COMPONENT_SCRIPT,
		Xen::StringValues::COMPONENT_POINT_LIGHT,
		Xen::StringValues::COMPONENT_AMBIENT_LIGHT,
		Xen::StringValues::COMPONENT_RIGID_BODY_2D,
		Xen::StringValues::COMPONENT_BOX_COLLIDER_2D,
		Xen::StringValues::COMPONENT_CIRCLE_COLLIDER_2D,
		Xen::StringValues::COMPONENT_PARTICLE_SYSTEM_2D
	};

	std::vector<std::string> m_AvailableComponents = m_Components;

	Xen::Entity m_SelectedEntity;
	std::string m_PanelTitle = Xen::StringValues::PANEL_TITLE_PROPERTIES;

	std::string m_TextureLoadDropType;
	std::string m_ScriptLoadDropType;


	ImGradientHDRState state;
	ImGradientHDRTemporaryState tempState;

	float p = 0.0f;
	float f = 200.0f;
};