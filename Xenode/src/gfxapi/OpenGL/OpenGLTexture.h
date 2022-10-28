#include "core/renderer/Texture.h"

namespace Xen {
	class XEN_API OpenGLTexture : public Texture2D
	{
	private:
		std::string m_FilePath;
		uint32_t m_TextureID;
		uint8_t m_TextureSlot;

		uint32_t m_Width, m_Height;
		uint8_t m_Channels;

		TextureWrapMode m_T_WrapMode = TextureWrapMode::Repeat;
		TextureFilterMode m_T_FilterMode = TextureFilterMode::Linear;

		uint8_t* m_TextureData;

	public:
		OpenGLTexture(const std::string& textureFilePath, uint8_t texture_slot, bool flip_on_load);
		virtual ~OpenGLTexture();

		void LoadTexture() override;

		void SetTextureWrapMode(TextureWrapMode mode) override;
		void SetTextureFilterMode(TextureFilterMode mode) override;

		inline uint32_t GetWidth() const override		{ return m_Width; }
		inline uint32_t GetHeight() const override		{ return m_Height; }

		inline uint8_t GetChannelCount() const override { return m_Channels; }

		void Bind() const override;

	public:
		bool store_in_cpu_mem = 0;
	};
}