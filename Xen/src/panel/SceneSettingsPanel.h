#pragma once

#include "Core.h"
#include "core/scene/Scene.h"
#include "core/scene/Components.h"
#include "imgui.h"

#include "core/app/Log.h"

#include "StringValues.h"

class SceneSettingsPanel {

public:
	SceneSettingsPanel() {}
	~SceneSettingsPanel() {}

	inline void SetPanelTitle(const std::string& title) { m_PanelTitle = title; }
	inline const std::string& GetPanelTitle() { return m_PanelTitle; }

	void OnImGuiRender()
	{
		ImGui::Begin(m_PanelTitle.c_str());
		ImGui::Text("This is going to be The Scene Settings panel!");
		ImGui::End();
	}

private:
	std::string m_PanelTitle = Xen::StringValues::PANEL_TITLE_SCENE_SETTINGS;
};