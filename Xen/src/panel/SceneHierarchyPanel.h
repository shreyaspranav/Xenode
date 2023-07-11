#pragma once

#include "Core.h"
#include "core/scene/Scene.h"
#include "core/scene/Components.h"
#include "imgui.h"

#include "core/app/Log.h"

#include "StringValues.h"

class SceneHierarchyPanel {

public:
	SceneHierarchyPanel() {}
	SceneHierarchyPanel(const Xen::Ref<Xen::Scene>& scene) : m_Scene(scene) {}
	~SceneHierarchyPanel() {}

	inline void SetActiveScene(const Xen::Ref<Xen::Scene>& scene) { m_Scene = scene; }
	inline void SetPanelTitle(const std::string& title) { m_PanelTitle = title; }

	inline const std::string& GetPanelTitle() { return m_PanelTitle; }
	inline const Xen::Entity& GetSelectedEntity() { return m_SelectedEntity; }
	inline const void SetSelectedEntity(const Xen::Entity& entity) { m_SelectedEntity = entity; }

	void OnImGuiRender()
	{
		ImGui::Begin(m_PanelTitle.c_str());

		if (ImGui::Button(ICON_FA_CIRCLE_PLUS))
			m_Scene->CreateEntity("Unnamed");
		ImGui::SameLine();

		ImGui::PushItemWidth(-30.0f);
		ImGui::InputTextWithHint(ICON_FA_MAGNIFYING_GLASS, "Search for entities", search_buf, IM_ARRAYSIZE(search_buf)); ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::NewLine();

		ImGui::Separator();

		m_Scene->m_Registry.each([&](auto& entity) 
			{
				Xen::Entity entt = Xen::Entity(entity, m_Scene.get());  
				DrawNode(entt);
			});

		if (ImGui::BeginPopup("DeleteEntity"))
		{
			if (ImGui::Selectable("Clone Entity"))
				m_Scene->CopyEntity(m_CurrentRightClickedEntity);

			ImGui::Separator();

			if (ImGui::Selectable("Delete"))
				m_Scene->DestroyEntity(m_CurrentRightClickedEntity);

			ImGui::EndPopup();
		}

		ImGui::End();
		//ImGui::ShowDemoWindow();
	}
private:
	void DrawNode(Xen::Entity entity)
	{
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow;

		Xen::Component::Tag& en_tag = entity.GetComponent<Xen::Component::Tag>();

		bool entity_tree_node_opened = ImGui::TreeNodeEx((std::string(ICON_FA_CUBE) + std::string(" ") + en_tag.tag).c_str(), base_flags);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("DeleteEntity");
			m_CurrentRightClickedEntity = entity;
			m_SelectedEntity = Xen::Entity();
		}

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			m_SelectedEntity = Xen::Entity();

		if (entity_tree_node_opened)
		{
			base_flags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			if (entity.HasAnyComponent<Xen::Component::SpriteRenderer>())
			{
				ImGui::TreeNodeEx(Xen::StringValues::COMPONENT_SPRITE_RENDERER.c_str(), base_flags);
				if(ImGui::IsItemClicked())
					m_SelectedEntity = entity;
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedEntity = Xen::Entity();
			}

			if (entity.HasAnyComponent<Xen::Component::NativeScript>())
			{
				ImGui::TreeNodeEx(Xen::StringValues::COMPONENT_NATIVE_SCRIPT.c_str(), base_flags);
				if (ImGui::IsItemClicked())
					m_SelectedEntity = entity;
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedEntity = Xen::Entity();
			}

			if (entity.HasAnyComponent<Xen::Component::CameraComp>())
			{
				ImGui::TreeNodeEx(Xen::StringValues::COMPONENT_CAMERA.c_str(), base_flags);
				if (ImGui::IsItemClicked())
					m_SelectedEntity = entity;
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedEntity = Xen::Entity();
			}

			if (entity.HasAnyComponent<Xen::Component::PointLight>())
			{
				ImGui::TreeNodeEx(Xen::StringValues::COMPONENT_POINT_LIGHT.c_str(), base_flags);
				if (ImGui::IsItemClicked())
					m_SelectedEntity = entity;
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedEntity = Xen::Entity();
			}

			if (entity.HasAnyComponent<Xen::Component::RigidBody2D>())
			{
				ImGui::TreeNodeEx(Xen::StringValues::COMPONENT_RIGID_BODY_2D.c_str(), base_flags);
				if (ImGui::IsItemClicked())
					m_SelectedEntity = entity;
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedEntity = Xen::Entity();
			}

			if (entity.HasAnyComponent<Xen::Component::BoxCollider2D>())
			{
				ImGui::TreeNodeEx(Xen::StringValues::COMPONENT_BOX_COLLIDER_2D.c_str(), base_flags);
				if (ImGui::IsItemClicked())
					m_SelectedEntity = entity;
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedEntity = Xen::Entity();
			}

			ImGui::TreePop();
		}
	}

private:
	char search_buf[128] = "";

	Xen::Entity m_SelectedEntity;

	Xen::Entity m_CurrentRightClickedEntity;

	Xen::Ref<Xen::Scene> m_Scene;
	std::string m_PanelTitle = Xen::StringValues::PANEL_TITLE_SCENE_HIERARCHY;

	bool m_DeleteEntityDisplayed = 0;
};