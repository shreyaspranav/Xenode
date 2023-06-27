#pragma once

#include <Core.h>

namespace Xen {

	enum class TextureWrapMode		{ Repeat, MirroredRepeat, ClampToEdge, ClampToBorder };
	enum class TextureFilterMode	{ Linear, Nearest };

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

		virtual inline uint8_t GetChannelCount() const = 0;

		virtual inline uint32_t GetNativeTextureID() const = 0;

		static Ref<Texture2D> CreateTexture2D(const std::string& filepath, bool flip_on_load);
		static Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, void* data, uint32_t size);

		static void BindTexture(uint32_t id, uint8_t slot);

		virtual bool operator==(const Ref<Texture2D> texture) const = 0;
	};
}