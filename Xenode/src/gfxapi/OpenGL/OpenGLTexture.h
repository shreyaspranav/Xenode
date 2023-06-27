#include "core/renderer/Texture.h"

namespace Xen {
	class XEN_API OpenGLTexture : public Texture2D
	{
	private:
		std::string m_FilePath;
		uint32_t m_TextureID;

		uint32_t m_Width, m_Height;
		uint8_t m_Channels;

		TextureWrapMode m_T_WrapMode = TextureWrapMode::Repeat;
		TextureFilterMode m_T_FilterMode = TextureFilterMode::Linear;

		uint8_t* m_TextureData;

	public:
		OpenGLTexture(const std::string& textureFilePath, bool flip_on_load);
		OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size);
		OpenGLTexture(uint32_t rendererID);

		virtual ~OpenGLTexture();

		void LoadTexture() override;

		void SetTextureWrapMode(TextureWrapMode mode) override;
		void SetTextureFilterMode(TextureFilterMode mode) override;

		inline uint32_t GetNativeTextureID() const override { return m_TextureID; }

		inline uint32_t GetWidth() const override		{ return m_Width; }
		inline uint32_t GetHeight() const override		{ return m_Height; }
		inline const std::string& GetFilePath() const override { return m_FilePath; }

		inline uint8_t GetChannelCount() const override { return m_Channels; }

		void Bind(uint8_t slot) const override;

		static void BindTextureExtID(uint32_t id, uint8_t slot);

		bool operator==(const Ref<Texture2D> texture) const override { return texture->GetNativeTextureID() == m_TextureID ? true : false; }

	public:
		bool store_in_cpu_mem = 0;
	};
}