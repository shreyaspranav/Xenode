#pragma once

#include "Core.h"
#include "imgui.h"

#include "core/app/Log.h"

#include "StringValues.h"
#include "core/scene/Scene.h"

#include "core/scene/SceneRuntime.h"

class SceneSettingsPanel {

public:
	SceneSettingsPanel() {}
	SceneSettingsPanel(const Xen::Ref<Xen::Scene>& scene, Xen::SceneSettings* sceneSettings)
		:m_Scene(scene), m_SceneSettings(sceneSettings){}
	~SceneSettingsPanel() {}

	inline void SetPanelTitle(const std::string& title) { m_PanelTitle = title; }
	inline const std::string& GetPanelTitle() { return m_PanelTitle; }

	void OnImGuiRender()
	{
		ImGui::Begin(m_PanelTitle.c_str());

#ifdef XEN_ENABLE_DEBUG_RENDERER
		Xen::SceneDebugSettings& debugSettings = m_SceneSettings->debugSettings;

		bool enablePhysicsColliders			= static_cast<bool>(debugSettings.physicsColliderTargetFlag & Xen::DebugRenderTargetFlag::Editor);
		bool enablePhysicsCollidersRuntime	= static_cast<bool>(debugSettings.physicsColliderTargetFlag & Xen::DebugRenderTargetFlag::Runtime);

		ImGui::SeparatorText("Physics Settings");

		if (ImGui::Checkbox("Show Physics Colliders", &enablePhysicsColliders))
		{
			enablePhysicsColliders ? 
				debugSettings.physicsColliderTargetFlag |=  Xen::DebugRenderTargetFlag::Editor :
				debugSettings.physicsColliderTargetFlag &= ~Xen::DebugRenderTargetFlag::Editor;
		}

		if (ImGui::Checkbox("Show Physics Colliders At Runtime", &enablePhysicsCollidersRuntime))
		{
			enablePhysicsCollidersRuntime ?
				debugSettings.physicsColliderTargetFlag |=  Xen::DebugRenderTargetFlag::Runtime :
				debugSettings.physicsColliderTargetFlag &= ~Xen::DebugRenderTargetFlag::Runtime;
		}

		float physicsColliderColor[] = 
		{ 
			debugSettings.physicsColliderColor.r, 
			debugSettings.physicsColliderColor.g, 
			debugSettings.physicsColliderColor.b 
		};

		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;

		if (ImGui::ColorEdit3("Physics Collider Color", physicsColliderColor, flags))
		{
			debugSettings.physicsColliderColor.r = physicsColliderColor[0];
			debugSettings.physicsColliderColor.g = physicsColliderColor[1];
			debugSettings.physicsColliderColor.b = physicsColliderColor[2];
		}

		ImGui::SeparatorText("FPS Overlay");
		ImGui::Checkbox("Show FPS Overlay", &m_SceneSettings->debugSettings.showFPSOverlay);
#else
		ImGui::Text("The Debug Renderer is not included!");
#endif
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
	Xen::SceneSettings* m_SceneSettings;
};