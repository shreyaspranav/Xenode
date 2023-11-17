#pragma once

#include "Core.h"
#include "imgui.h"

#include "core/app/Log.h"

#include "StringValues.h"
#include "core/scene/Scene.h"

class SceneSettingsPanel {

public:
	SceneSettingsPanel() {}
	SceneSettingsPanel(const Xen::Ref<Xen::Scene>& scene) 
		:m_Scene(scene){}
	~SceneSettingsPanel() {}

	inline void SetPanelTitle(const std::string& title) { m_PanelTitle = title; }
	inline const std::string& GetPanelTitle() { return m_PanelTitle; }

	void OnImGuiRender()
	{
		ImGui::Begin(m_PanelTitle.c_str());

		ImGui::Columns(2, "##SceneSettings", false);
		//ImGui::SetColumnWidth(1, 100.0f);
		ImGui::SetColumnWidth(0, 245.0f);

		PaddedText("Show Physics Colliders", 0.0f, 3.0f);
		ImGui::NextColumn();

		bool enablePhysicsColliders = m_Scene->IsPhysicsCollidersShown();
		bool enablePhysicsCollidersRuntime = m_Scene->IsPhysicsCollidersRuntimeShown();

		ImGui::PushItemWidth(-1.0f);

		if (ImGui::Checkbox("##ShowPhysicsColliders", &enablePhysicsColliders))
			m_Scene->ShowPhysicsColliders(enablePhysicsColliders);

		ImGui::PopItemWidth();

		ImGui::NextColumn();

		PaddedText("Show Physics Colliders(Runtime)", 0.0f, 3.0f);
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1.0f);

		if (ImGui::Checkbox("##ShowPhysicsCollidersRuntime", &enablePhysicsCollidersRuntime))
			m_Scene->ShowPhysicsCollidersRuntime(enablePhysicsCollidersRuntime);

		ImGui::PopItemWidth();

		ImGui::Columns(1);

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

private:
	std::string m_PanelTitle = Xen::StringValues::PANEL_TITLE_SCENE_SETTINGS;

	Xen::Ref<Xen::Scene> m_Scene;
};