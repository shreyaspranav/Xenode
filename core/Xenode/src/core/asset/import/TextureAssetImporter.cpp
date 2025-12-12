#include "pch"
#include "TextureAssetImporter.h"

#include <core/app/Timer.h>
#include <core/renderer/Texture.h>

#include <project/ProjectManager.h>

#include <stb_image.h>

namespace Xen
{
	Vector<std::byte> TextureAssetImporter::ImportTextureAsset(AssetMetadata* metadata)
	{
		// This shouldn't happen in runtime!
		EditorAssetMetadata* editorAssetMetadata = (EditorAssetMetadata*)metadata;

		Ref<Project> currentProject = ProjectManager::GetCurrentProject();
		std::filesystem::path assetPath = ProjectManager::GetCurrentProjectPath() / currentProject->GetProjectSettings().relAssetDirectory;

		assetPath /= editorAssetMetadata->relPath;

		// TODO: Add more texture types if you want.
		if (metadata->type == AssetType::Texture2D) 
			return ImportTexture2D(metadata, assetPath);

		XEN_ENGINE_LOG_ERROR("Unknown Texture Type: {0}", assetPath.string());
		return Vector<std::byte>();
	}

	Ref<Asset> TextureAssetImporter::LoadTextureAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata)
	{
		TextureMetadata& textureMetadata = std::get<TextureMetadata>(metadata->specific);
		Ref<Texture2D> textureAsset = Texture2D::CreateTexture2D(buffer, textureMetadata.bufferType, textureMetadata.properties);

		return textureAsset;
	}
	
	// Will load only for non floating point textures
	Vector<std::byte> TextureAssetImporter::ImportTexture2D(AssetMetadata* metadata, const std::filesystem::path& completeFilePath)
	{
		// This shouldn't happen in runtime!
		EditorAssetMetadata* editorAssetMetadata = (EditorAssetMetadata*)metadata;

		std::string filePathString = completeFilePath.string();

		// Check the type of data in the image file.
		TextureBufferType dataType = TextureBufferType::UnsignedInt8;

		if (stbi_is_16_bit(filePathString.c_str()))
			dataType = TextureBufferType::UnsignedInt16;

		int width = 0, height = 0, channels = 0;
		Vector<std::byte> textureDataBuffer;
		std::byte* data = nullptr;

		constexpr bool flipTextureOnLoad = false;
		stbi_set_flip_vertically_on_load(flipTextureOnLoad);

		// Read the texture data
		switch (dataType)
		{
		// TODO: Look into the 'desired channels' parameter, maybe extend some features based on it.
		case TextureBufferType::UnsignedInt8:
			data = (std::byte*)stbi_load(filePathString.c_str(), &width, &height, &channels, 0);
			if (!data) break;
			
			textureDataBuffer.resize(width * height * channels * sizeof(uint8_t));
			std::memcpy(textureDataBuffer.data(), data, textureDataBuffer.size());
			
			break;
		case TextureBufferType::UnsignedInt16:
			data = (std::byte*)stbi_load_16(filePathString.c_str(), &width, &height, &channels, 0);
			if (!data) break;

			textureDataBuffer.resize(width * height * channels * sizeof(uint16_t));
			std::memcpy(textureDataBuffer.data(), data, textureDataBuffer.size());
			
			break;
		}

		if (textureDataBuffer.empty())
		{
			XEN_ENGINE_LOG_ERROR("Failed to import {0} as Texture2D", filePathString);
			return Vector<std::byte>();
		}

		TextureProperties textureProperties;
		textureProperties.width = width;
		textureProperties.height = height;

		switch (channels)
		{
		case 1: textureProperties.format = dataType == TextureBufferType::UnsignedInt16 ? TextureFormat::G16 : TextureFormat::G8; break;
		case 2: textureProperties.format = dataType == TextureBufferType::UnsignedInt16 ? TextureFormat::GA16 : TextureFormat::GA8; break;
		case 3: textureProperties.format = dataType == TextureBufferType::UnsignedInt16 ? TextureFormat::RGB16 : TextureFormat::RGB8; break;
		case 4: textureProperties.format = dataType == TextureBufferType::UnsignedInt16 ? TextureFormat::RGBA16 : TextureFormat::RGBA8; break;
		}

		editorAssetMetadata->specific = TextureMetadata{ textureProperties, dataType };
		return textureDataBuffer;
	}
}