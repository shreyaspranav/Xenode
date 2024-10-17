#include "pch"
#include "TextureAssetImporter.h"

#include <core/app/Timer.h>
#include <core/renderer/Texture.h>
#include <project/ProjectManager.h>

#include <stb_image.h>

namespace Xen
{
	Ref<Asset> TextureAssetImporter::ImportTextureAsset(AssetMetadata* metadata)
	{
		Ref<Project> currentProject = ProjectManager::GetCurrentProject();
		std::filesystem::path assetPath = ProjectManager::GetCurrentProjectPath() / currentProject->GetProjectSettings().relAssetDirectory;

		assetPath /= metadata->relPath;

		// TODO: Add more texture types if you want.
		if (metadata->type == AssetType::Texture2D) 
			return ImportTexture2D(metadata, assetPath);
	}
	
	// Will load only for non floating point textures
	Ref<Asset> TextureAssetImporter::ImportTexture2D(AssetMetadata* metadata, const std::filesystem::path& completeFilePath)
	{
		std::string filePathString = completeFilePath.string();

		// Check the type of data in the image file.
		TextureBufferType dataType = TextureBufferType::UnsignedInt8;

		if (stbi_is_16_bit(filePathString.c_str()))
			dataType = TextureBufferType::UnsignedInt16;

		int width, height, channels;
		Buffer textureDataBuffer;

		// Read the texture data
		switch (dataType)
		{
		case TextureBufferType::UnsignedInt8:
			// TODO: Look into the 'desired channels' parameter, maybe extend some features based on it.
			textureDataBuffer.buffer = stbi_load(filePathString.c_str(), &width, &height, &channels, 0);
			break;
		case TextureBufferType::UnsignedInt16:
			// TODO: Look into the 'desired channels' parameter, maybe extend some features based on it.
			textureDataBuffer.buffer = stbi_load_16(filePathString.c_str(), &width, &height, &channels, 0);
			break;
		}

		if (!textureDataBuffer.buffer)
		{
			XEN_ENGINE_LOG_ERROR("Failed to import {0} as Texture2D", filePathString);
			return nullptr;
		}

		switch (dataType)
		{
		case TextureBufferType::UnsignedInt8:
			textureDataBuffer.size = width * height * channels * sizeof(uint8_t);
			break;
		case TextureBufferType::UnsignedInt16:
			textureDataBuffer.size = width * height * channels * sizeof(uint16_t);
			break;
		}

		textureDataBuffer.alloc = true;

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

		Ref<Texture2D> textureAsset = Texture2D::CreateTexture2D(textureDataBuffer, dataType, textureProperties);
		
		// Make sure the memory gets cleared.
		metadata->size = textureDataBuffer.size;
		stbi_image_free(textureDataBuffer.buffer);
		textureDataBuffer.alloc = false;

		return textureAsset;
	}
}