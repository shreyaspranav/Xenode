#pragma once

#include "Core.h"
#include "imgui.h"

#include "core/app/Log.h"

#include "StringValues.h"
#include "core/renderer/ScreenRenderer.h"

class SceneSettingsPanel {

public:
	SceneSettingsPanel() {}
	~SceneSettingsPanel() {}

	inline void SetPanelTitle(const std::string& title) { m_PanelTitle = title; }
	inline const std::string& GetPanelTitle() { return m_PanelTitle; }

	void OnImGuiRender()
	{
#if 0
		ImGui::Begin(m_PanelTitle.c_str());

		ImGui::Columns(2, "##SceneSettings", false);
		ImGui::SetColumnWidth(0, 100.0f);

		PaddedText("Ambient Light", 0.0f, 3.0f);
		ImGui::NextColumn();

		float ambientLight = Xen::ScreenRenderer2D::GetAmbientLightIntensity();

		ImGui::PushItemWidth(-0.1f);
		if (ImGui::SliderFloat("##AmbientLight", &ambientLight, 0.0f, 1.0f))
			Xen::ScreenRenderer2D::SetAmbientLightIntensity(ambientLight);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::End();
#endif
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
};