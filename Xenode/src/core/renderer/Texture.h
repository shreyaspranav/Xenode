#pragma once

#include <Core.h>

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
		RGB8, RGB16, RGB16F, RGB32F,

		// RGBA(Red Green Blue, Alpha) Images(F is for floating point textures)
		RGBA8, RGBA16, RGBA16F, RGBA32F
	};

	struct TextureProperties 
	{
		uint32_t width, height;
		TextureFormat format;
	};

	class Texture 
	{
	public:
		virtual inline uint32_t GetWidth() const = 0;
		virtual inline uint32_t GetHeight() const = 0;
		virtual inline const std::string& GetFilePath() const = 0;

		virtual void Bind(uint8_t slot) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual void LoadTexture() = 0;

		virtual void SetTextureWrapMode(TextureWrapMode mode) = 0;
		virtual void SetTextureFilterMode(TextureFilterMode mode) = 0;

		virtual inline TextureProperties GetTextureProperties() const = 0;
		virtual inline uint32_t GetNativeTextureID() const = 0;

		static Ref<Texture2D> CreateTexture2D(const std::string& filepath, bool flip_on_load);
		static Ref<Texture2D> CreateTexture2D(TextureProperties properties, void* data, uint32_t size);

		static void BindTexture(uint32_t id, uint8_t slot);
		static void BindToImageUnit(const Ref<Texture2D>& texture, uint8_t slot);

		virtual bool operator==(const Ref<Texture2D> texture) const = 0;
	};
}