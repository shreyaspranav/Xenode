#pragma once

#include <Core.h>
#include <imgui.h>
#include <imgui/IconsFontAwesome.h>

#include <core/renderer/Texture.h>
#include <core/app/GameApplication.h>

#include <core/asset/AssetManagerUtil.h>
#include <core/asset/AssetUserData.h>

#include "StringValues.h"
#include "ThumbnailGenerator.h"

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

			// To flip the thumbnails, toggle this.
			// To flip selectively according to type of asset thumbnail, toggle individually in the if ... else if block below.
			bool flipThumbnail = false;

			Xen::Ref<Xen::Texture2D> thumbnail;
			if (childrenNode->type == Xen::AssetHandleFileTreeNodeType::Folder)
				thumbnail = m_FolderTexture;
			else if (assetMetadataRegistry[childrenNode->handle].type == Xen::AssetType::Texture2D)
			{
				Xen::TextureAssetUserData* textureAssetUserData = (Xen::TextureAssetUserData*)assetMetadataRegistry[childrenNode->handle].userData.buffer;
				thumbnail = textureAssetUserData->thumbnail;
			}
			else if (assetMetadataRegistry[childrenNode->handle].type == Xen::AssetType::Scene)
			{
				Xen::SceneAssetUserData* sceneAssetUserData = (Xen::SceneAssetUserData*)assetMetadataRegistry[childrenNode->handle].userData.buffer;
				thumbnail = sceneAssetUserData->thumbnail;
				flipThumbnail = true;
			}
			else
				// use the file texture for all the other types of assets.
				// TODO: Improve this:
				thumbnail = m_FileTexture;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			childrenNode->type == Xen::AssetHandleFileTreeNodeType::Folder ? 
				ImGui::PushID(childrenNode->folderName.c_str()) : ImGui::PushID((uint32_t)childrenNode->handle);

			{
				Xen::TextureProperties thumbnailProperties = thumbnail->GetTextureProperties();
				float thumbnailAspectRatio = static_cast<float>(thumbnailProperties.width) / static_cast<float>(thumbnailProperties.height);

				// Calculate the UV coordinates according to the aspect ratio, adjust them when the thumbnail needs to be flipped.
				ImVec2 uv0 = { 0.0f, flipThumbnail ? 0.5f + thumbnailAspectRatio / 2 : 0.5f - thumbnailAspectRatio / 2 };
				ImVec2 uv1 = { 1.0f, flipThumbnail ? 0.5f - thumbnailAspectRatio / 2 : 0.5f + thumbnailAspectRatio / 2 };

				// Display the thumbnail as a ImageButton
				ImGui::ImageButton((ImTextureID)(thumbnail->GetNativeTextureID()), { static_cast<float>(m_IconSize), static_cast<float>(m_IconSize) }, uv0, uv1);
			}


			if (ImGui::BeginDragDropSource())
			{
				switch (assetMetadataRegistry[childrenNode->handle].type)
				{
				case Xen::AssetType::Scene:
					ImGui::SetDragDropPayload(m_SceneLoadDropType.c_str(), &childrenNode->handle, sizeof(Xen::AssetHandle));
					break;
				case Xen::AssetType::Texture2D:
					ImGui::SetDragDropPayload(m_TextureLoadDropType.c_str(), &childrenNode->handle, sizeof(Xen::AssetHandle));
				}
				ImGui::EndDragDropSource();
			}

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