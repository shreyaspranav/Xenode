#pragma once

#include <Core.h>
#include <imgui.h>
#include <imgui/IconsFontAwesome.h>

class ContentBrowserPanel 
{
public:
	ContentBrowserPanel() {}
	~ContentBrowserPanel() {}

	void OnImGuiEditor()
	{
		ImGui::Begin(m_PanelTitle.c_str());
		ImGui::Text("This is going to the content browser panel!");
		ImGui::End();
	}

	const std::string& GetPanelTitle(){ return m_PanelTitle; }

private:
	std::string m_PanelTitle = std::string(ICON_FA_FOLDER) + std::string(" Content Browser");
};