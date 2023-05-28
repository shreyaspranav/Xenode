#pragma once

#include <Core.h>
#include <imgui.h>
#include <imgui/IconsFontAwesome.h>

#include <core/renderer/Texture.h>

class ContentBrowserPanel 
{
public:
	ContentBrowserPanel() {}
	~ContentBrowserPanel() {}

	void OnImGuiEditor()
	{

		// Load Textures of the icons here:
		if (!m_LoadedTextures)
		{
			m_FileTexture = Xen::Texture2D::CreateTexture2D("assets/textures/file.png", false);
			m_FolderTexture = Xen::Texture2D::CreateTexture2D("assets/textures/folder.png", false);
			m_PngTexture = Xen::Texture2D::CreateTexture2D("assets/textures/png.png", false);
			
			m_FileTexture->LoadTexture();
			m_FolderTexture->LoadTexture();
			m_PngTexture->LoadTexture();

			m_LoadedTextures = true;
		}

		// The header part of the panel -------------------------------------
		ImGui::Begin(m_PanelTitle.c_str());

		bool up_button_disabled = false;

		if (m_CurrentPath != m_AssetsPath)
			up_button_disabled = true;

		ImGui::PushDisabled(!up_button_disabled);
		if(ImGui::Button(m_BackIcon.c_str()))
			m_CurrentPath = m_CurrentPath.parent_path();
		ImGui::PopDisabled();

		ImGui::SameLine();

		float windowWidth = ImGui::GetContentRegionAvail().x;
		float textFieldWidth = 375.0f;

		ImGui::SetCursorPosX(windowWidth - textFieldWidth);

		ImGui::PushItemWidth(textFieldWidth);
		ImGui::InputTextWithHint(m_SearchIcon.c_str(), "Search in assets", m_SearchBuf, 40);
		ImGui::PopItemWidth();
		// -------------------------------------------------------------------

		ImGui::Separator();

		uint32_t panel_width = ImGui::GetContentRegionAvail().x;
		uint32_t cell_size = m_IconSize + m_IconPadding;

		uint32_t column_count = (uint32_t)(panel_width / cell_size);
		if (column_count < 1)
			column_count = 1;

		ImGui::Columns(column_count, "##FileColumns", false);

		for (auto& p : std::filesystem::directory_iterator{ m_CurrentPath })
		{
			const auto& path = p.path();
			std::string pathString = p.path().string();
			std::string fileName = p.path().filename().string();

			Xen::Ref<Xen::Texture2D> icon_to_show = p.is_directory() ? m_FolderTexture : m_FileTexture;

			// Temporary. Load the Texture and display that as the icon instead!
			if (path.extension().string() == ".png")
				icon_to_show = m_PngTexture;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::PushID(pathString.c_str());
			ImGui::ImageButton((ImTextureID)(icon_to_show->GetNativeTextureID()), { (float)m_IconSize, (float)m_IconSize });

			if (ImGui::BeginDragDropSource())
			{
				const char* payload_data = pathString.c_str();
				if (path.extension().string() == ".xen")
					ImGui::SetDragDropPayload(m_SceneLoadDropType.c_str(), payload_data, pathString.size() + 1);

				// TODO: make sure to support all the texture formats:
				else if (path.extension().string() == ".png")
					ImGui::SetDragDropPayload(m_TextureLoadDropType.c_str(), payload_data, pathString.size() + 1);

				ImGui::EndDragDropSource();
			}

			ImGui::PopID();

			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && p.is_directory())
				m_CurrentPath /= path.filename();

			ImGui::TextWrapped(fileName.c_str());
			ImGui::PopStyleColor();
			ImGui::NextColumn();

		}
		ImGui::Columns(1);
		ImGui::End();
	}

	inline const std::string& GetPanelTitle() { return m_PanelTitle; }

	inline const std::string& GetSceneLoadDropType()	{ return m_SceneLoadDropType; }
	inline const std::string& GetTextureLoadDropType()	{ return m_TextureLoadDropType; }

private:
	std::string m_PanelTitle = std::string(ICON_FA_FOLDER) + std::string(" Content Browser");
	std::string m_BackIcon = std::string(ICON_FA_ARROW_UP);
	std::string m_SearchIcon = std::string(ICON_FA_MAGNIFYING_GLASS);
	
	// Hardcoded it to assets, change in future
	std::filesystem::path m_AssetsPath{ "assets" };
	std::filesystem::path m_CurrentPath{ "assets" };

	// Drag drop types:
	std::string m_SceneLoadDropType = "XEN_CONTENT_BROWSER_SCENE_LOAD";
	std::string m_TextureLoadDropType = "XEN_CONTENT_BROWSER_TEXTURE_LOAD";

	Xen::Ref<Xen::Texture2D> m_FolderTexture;
	Xen::Ref<Xen::Texture2D> m_FileTexture;

	// Temporary:
	Xen::Ref<Xen::Texture2D> m_PngTexture;

	uint32_t m_IconSize = 80;
	uint32_t m_IconPadding = 25;

	char m_SearchBuf[40] = "";

	bool m_LoadedTextures = false;
};