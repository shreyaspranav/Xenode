#pragma once

#include <Core.h>
#include <core/asset/Asset.h>

namespace Xen {

	enum class TextureWrapMode		{ Repeat, MirroredRepeat, ClampToEdge, ClampToBorder };
	enum class TextureFilterMode	{ Linear, Nearest };

	enum class TextureFormat : uint8_t
	{
		Unknown = 0,

		// Grey Scale Images
		G8, G16,

		// Grey Scale + Alpha Images
		GA8, GA16,

		// RGB(Red Green Blue) Images(F is for floating point textures)
		RGB8, RGB16, RGB16F, RGB32F, R11G11B10F,

		// RGBA(Red Green Blue, Alpha) Images(F is for floating point textures)
		RGBA8, RGBA16, RGBA16F, RGBA32F
	};

	// Determines the type of texture data decoded from file.
	enum class TextureBufferType
	{
		UnsignedInt8,   // uint8_t
		UnsignedInt16,  // uint16_t
		Float32         // float
	};

	struct TextureProperties 
	{
		uint32_t width, height;
		TextureFormat format;
		uint8_t mipLevels = 0;
	};

	class Texture 
	{
	public:
		virtual inline uint32_t GetWidth() const = 0;
		virtual inline uint32_t GetHeight() const = 0;
		virtual inline const std::string& GetFilePath() const = 0;

		virtual void Bind(uint8_t slot) const = 0;
	};

	class Texture2D : public Texture, public Asset
	{
	public:
		virtual void LoadTexture() = 0;

		virtual void SetTextureWrapMode(TextureWrapMode mode) = 0;
		virtual void SetTextureFilterMode(TextureFilterMode mode) = 0;

		virtual inline TextureProperties GetTextureProperties() const = 0;
		virtual inline uint32_t GetNativeTextureID() const = 0;

		static Ref<Texture2D> CreateTexture2D(const std::string& filepath, bool flip_on_load);
		static Ref<Texture2D> CreateTexture2D(TextureProperties properties, void* data, uint32_t size);

		static Ref<Texture2D> CreateTexture2D(const Buffer& textureBuffer, TextureBufferType bufferType, TextureProperties properties);

		static void BindTexture(uint32_t textureID, uint8_t slot);
		static void BindToImageUnit(const Ref<Texture2D>& texture, uint8_t slot, uint8_t mipLevel = 0);
		static void BindToImageUnit(uint32_t textureID, TextureFormat format, uint8_t slot, uint8_t mipLevel = 0);

		virtual bool operator==(const Ref<Texture2D> texture) const = 0;

		static Ref<Texture2D> CreateTexture2D(uint32_t rendererID, TextureProperties properties);

		DEFINE_ASSET_TYPE(AssetType::Texture2D);
	};
}