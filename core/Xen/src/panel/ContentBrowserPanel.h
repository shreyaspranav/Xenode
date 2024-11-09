#pragma once

#include <Core.h>
#include <imgui.h>
#include <imgui/IconsFontAwesome.h>

#include <core/renderer/Texture.h>
#include <core/app/GameApplication.h>

#include <core/asset/AssetManagerUtil.h>

#include "StringValues.h"

class ContentBrowserPanel 
{
public:
	ContentBrowserPanel() {}
	ContentBrowserPanel(const std::filesystem::path& assetsDirectory) 
		// :m_AssetsPath(assetsDirectory) 
	{
		// m_CurrentPath = m_AssetsPath;
		m_DisplayAssetHandleFileTree = Xen::AssetManagerUtil::GetEditorAssetManager()->GetAssetHandleFileTree();

	}

	~ContentBrowserPanel() {}

	void OnImGuiRender()
	{
		// Load Textures of the icons here:
		if (!m_LoadedTextures)
		{
			m_FileTexture = Xen::Texture2D::CreateTexture2D(std::string(EDITOR_RESOURCES) + "/textures/file.png", false);
			m_FolderTexture = Xen::Texture2D::CreateTexture2D(std::string(EDITOR_RESOURCES) + "/textures/folder.png", false);
			m_PngTexture = Xen::Texture2D::CreateTexture2D(std::string(EDITOR_RESOURCES) + "/textures/png.png", false);
			
			m_FileTexture->LoadTexture();
			m_FolderTexture->LoadTexture();
			m_PngTexture->LoadTexture();

			m_LoadedTextures = true;
		}

		// The header part of the panel -------------------------------------
		ImGui::Begin(m_PanelTitle.c_str());

		bool upButtonEnabled = true;

		if (m_DisplayAssetHandleFileTree->parent)
			upButtonEnabled = false;

		ImGui::BeginDisabled(upButtonEnabled);

		if (ImGui::Button(m_BackIcon.c_str()))
			m_DisplayAssetHandleFileTree = m_DisplayAssetHandleFileTree->parent;
		
		ImGui::EndDisabled();

		ImGui::SameLine();

		float windowWidth = ImGui::GetContentRegionAvail().x;
		float textFieldWidth = 375.0f;

		ImGui::SetCursorPosX(windowWidth - textFieldWidth);

		ImGui::PushItemWidth(textFieldWidth);
		ImGui::InputTextWithHint(m_SearchIcon.c_str(), "Search in assets", m_SearchBuf, 40);
		ImGui::PopItemWidth();
		// -------------------------------------------------------------------

		ImGui::Separator();

		uint32_t panelWidth = ImGui::GetContentRegionAvail().x;
		uint32_t cellSize = m_IconSize + m_IconPadding;

		uint32_t columnCount = (uint32_t)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, "##FileColumns", false);

#if 1
		Xen::AssetMetadataRegistry assetMetadataRegistry = Xen::AssetManagerUtil::GetEditorAssetManager()->GetAssetMetadataRegistry();

		for(int i = 0; i < m_DisplayAssetHandleFileTree->children.size(); i++)
		{
			Xen::AssetHandleFileTreeNode* childrenNode = m_DisplayAssetHandleFileTree->children[i];

			// TODO: Not all assets are textures, make sure to retrieve the texture type and then display the appropriate thumbnail.
			Xen::Ref<Xen::Texture2D> thumbnail =
				childrenNode->type == Xen::AssetHandleFileTreeNodeType::Folder ? m_FolderTexture :
				Xen::AssetManagerUtil::GetAsset<Xen::Texture2D>(childrenNode->handle);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			childrenNode->type == Xen::AssetHandleFileTreeNodeType::Folder ? 
				ImGui::PushID(childrenNode->folderName.c_str()) : ImGui::PushID((uint32_t)childrenNode->handle);

			ImGui::ImageButton((ImTextureID)(thumbnail->GetNativeTextureID()), { (float)m_IconSize, (float)m_IconSize });


			// if (ImGui::BeginDragDropSource())
			// {
			// 	const char* payload_data = pathString.c_str();
			// 	if (path.extension().string() == ".xen")
			// 		ImGui::SetDragDropPayload(m_SceneLoadDropType.c_str(), payload_data, pathString.size() + 1);
			// 
			// 	else if (path.extension().string() == ".lua" && Xen::GetApplicationInstance()->GetScriptLang() == Xen::ScriptLang::Lua)
			// 		ImGui::SetDragDropPayload(m_ScriptLoadDropType.c_str(), payload_data, pathString.size() + 1);
			// 
			// 	// TODO: make sure to support all the texture formats:
			// 	else if (path.extension().string() == ".png")
			// 		ImGui::SetDragDropPayload(m_TextureLoadDropType.c_str(), payload_data, pathString.size() + 1);
			// 
			// 	ImGui::EndDragDropSource();
			// }

			ImGui::PopID();

			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && childrenNode->type == Xen::AssetHandleFileTreeNodeType::Folder)
				m_DisplayAssetHandleFileTree = childrenNode;

			// ImGui::TextWrapped(fileName.c_str());

			childrenNode->type == Xen::AssetHandleFileTreeNodeType::Folder ?
				ImGui::TextWrapped(childrenNode->folderName.c_str()) : 
				ImGui::TextWrapped(assetMetadataRegistry[childrenNode->handle].relPath.filename().string().c_str());

			ImGui::PopStyleColor();
			ImGui::NextColumn();

		}
#endif
		ImGui::Columns(1);
		ImGui::End();
	}

	inline const std::string& GetPanelTitle() { return m_PanelTitle; }

	inline const std::string& GetSceneLoadDropType()	{ return m_SceneLoadDropType; }
	inline const std::string& GetTextureLoadDropType()	{ return m_TextureLoadDropType; }
	inline const std::string& GetScriptLoadDropType()	{ return m_ScriptLoadDropType; }

private:
	std::string m_PanelTitle = Xen::StringValues::PANEL_TITLE_CONTENT_BROWSER;
	std::string m_BackIcon = std::string(ICON_FA_ARROW_UP);
	std::string m_SearchIcon = std::string(ICON_FA_MAGNIFYING_GLASS);

	// Drag drop types:
	std::string m_SceneLoadDropType    = "XEN_CONTENT_BROWSER_SCENE_LOAD";
	std::string m_TextureLoadDropType  = "XEN_CONTENT_BROWSER_TEXTURE_LOAD";
	std::string m_ScriptLoadDropType   = "XEN_CONTENT_BROWSER_SCRIPT_LOAD";

	Xen::Ref<Xen::Texture2D> m_FolderTexture;
	Xen::Ref<Xen::Texture2D> m_FileTexture;

	// Temporary:
	Xen::Ref<Xen::Texture2D> m_PngTexture;

	uint32_t m_IconSize = 100;
	uint32_t m_IconPadding = 25;

	// TEMP: 
	Xen::AssetHandleFileTreeNode* m_DisplayAssetHandleFileTree;

	char m_SearchBuf[40] = "";

	bool m_LoadedTextures = false;
};